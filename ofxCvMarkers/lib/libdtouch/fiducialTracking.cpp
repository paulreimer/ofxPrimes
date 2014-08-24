#include "fiducialTracking.h"

#ifndef __SYMBIAN32__
#include <iostream>
#endif

FiducialTracking * FiducialTracking::newL(const int &distance,const int &tracking)
{
	FiducialTracking* self = new FiducialTracking(distance,tracking);
	self->initL();

	return self;
}

FiducialTracking::FiducialTracking(const int &distance,const int &tracking)
{
	_idCount=0;
	_distanceThreshold = distance;
	_trackingThreshold = tracking;

}

FiducialTracking::~FiducialTracking(void)
{

	if(!_pre->isEmpty())_pre->empty();
	if(!_filtered->isEmpty())_filtered->empty();
	delete _pre;
	delete _filtered;
}

void FiducialTracking::initL()
{
	_pre = FiducialDataList::newL();
	_filtered = FiducialDataList::newL();
}

/*
	WARNING: the following function assumes that the argument is NOT NULL!
*/
void FiducialTracking::process(FiducialDataList * curr)
{
	if(!_filtered->isEmpty()){
		_filtered->empty();
	}

	int currSize = curr->getSize();
	int prevSize = _pre->getSize();

	//flags used to separate between new, deleted and same markers in the previous and current list
	bool* currTracked = NULL;
	bool* prevTracked = NULL;
	currTracked = new bool[currSize];
	prevTracked = new bool[prevSize];

	for(int i=0;i<currSize;i++){
		currTracked[i]=false;
	}
	for(int j=0;j<prevSize;j++){
		prevTracked[j]=false;
	}


	//blocks are flagged if they appear in previous and current list within a close distance
	//TODO: check the type/sequence
	int previndex=0;
	for( _pre->reset(); !(_pre->nullTest()); _pre->fwd() ){
		FiducialData prevData = _pre->getData();
		DTPoint prevPoint = prevData.Centre();

		int closestIndex = -1;
		int closestDistance = 640;
		FiducialData closestData;

		int currIndex=0;
		for( curr->reset(); !(curr->nullTest()); curr->fwd() ){
			FiducialData currData = curr->getData();
			if(!currTracked[currIndex]){ //process only untracked blocks

				int distance = prevPoint.dist(currData.Centre());
				// the block is identified with a previous one

				if( distance < _distanceThreshold && (prevData.getType()==currData.getType()) ){
					if( distance < closestDistance ){
						closestDistance = distance;
						closestIndex = currIndex;
						closestData = currData;
					}
				}
			}
			currIndex++;

		}

		if( closestIndex > -1 ){
			//std::cout << "prev type " << prevData.getType() << " curr type " << currData.getType() << " distance " << distance << std::endl;
			int prevID = prevData.getID();
			closestData.setID(prevID);
			closestData.setTracked(_trackingThreshold); //resetting the time out
			_filtered->appendL(closestData);
			currTracked[closestIndex]=true;
			prevTracked[previndex]=true;
		}
		previndex++;

	}

	//not tracked in the current list means new blocks
	int currindex=0;
	for( curr->reset(); !(curr->nullTest()); curr->fwd() ){
		if(!currTracked[currindex]){
			FiducialData currData = curr->getData();
			//makes really really sure that there is no one near in order to prevent overgrowing. Not sure if necessary
			bool near=false;
			previndex = 0;
			for(_pre->reset(); !(_pre->nullTest()); _pre->fwd()){
				if(!prevTracked[previndex]){
					FiducialData prevData = _pre->getData();
					int distance = prevData.Centre().dist(currData.Centre());
					if( distance < _distanceThreshold && (prevData.getType()==currData.getType()) ){
						near=true;
					}

				}
				previndex++;

			}

			if(!near){

			//generate a new ID
			currData.setID(generateID());
			currData.setTracked(_trackingThreshold);
			_filtered->appendL(currData);
			#ifdef _EC_DEBUG
			std::cout << "created ID: " << currData.getID() << " with type: " << currData.getType() << std::endl;
			#endif
			}
			else{
//#ifdef _EC_DEBUG
				std::cout << " catched someone near"<< std::endl;
//#endif
			}
		}
		currindex++;
	}

	//not tracked in the previous list means deleted blocks
	previndex=0;
	for( _pre->reset(); !(_pre->nullTest()); _pre->fwd() ){
		if(!prevTracked[previndex]){
		//deleted blocks get rescued for a certain amount of time to prevent short unwanted occlusions that would cause jittering
		FiducialData delData = _pre->getData();
		int rescue_counter = delData.getTracked();
		if(rescue_counter>1){
			rescue_counter--;
			delData.setTracked(rescue_counter);
			_filtered->appendL(delData);
#ifdef _EC_DEBUG
			std::cout << "rescued counter: " << rescue_counter <<std::endl;
#endif
		}
		else{
#ifdef _EC_DEBUG
			std::cout << "deleted ID: " << delData.getID()<< " with type: " << delData.getType() <<std::endl;
#endif
		}

		}
		previndex++;
	}

	if(!curr->isEmpty()){
		curr->empty();
	}

	for( _filtered->reset(); !(_filtered->nullTest()); _filtered->fwd() ){
		FiducialData filData = _filtered->getData();
		curr->appendL(filData);
	}

	if(!_pre->isEmpty()){
		_pre->empty();
	}

	_pre->append(_filtered);

	if(currTracked) delete[] currTracked;
	if(prevTracked) delete[] prevTracked;

}

int FiducialTracking::generateID()
{
	_idCount++;
	if (_idCount > 10000){
		_idCount = 0;
	}
	//TODO: check for overflow?
	return _idCount;
}