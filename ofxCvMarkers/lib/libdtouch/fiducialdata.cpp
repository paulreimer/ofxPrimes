/***************************************************************************
   fiducialdata.cpp  -  container class for fiducial recognition results
                             -------------------
    begin                : Sat Nov 27 2004
    copyright            : (C) 2004 by Enrico Costanza
    email                : e.costanza@ieee.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *	This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330                           *
 *  Boston, MA  02111-1307  USA                                            *
 *                                                                         *
 ***************************************************************************/



#include "fiducialdata.h"
#include "intlist.h"
#include <assert.h>

//#include <iostream>
//using std::cout;
//using std::endl;

#ifdef __SYMBIAN32__
#include <string.h>
#endif

FiducialData::FiducialData(void) :
	_type(0),
	_centre(0,0),
	_width(0),
	_height(0),
	_angle(0),
	_rootRegion(0),
	_whiteRoot(false),
	_size(0),
	_ID(0),
	_tracked(0)
{
	_sequence[0] = 0;
	_typeArray[0] = 0;
	for(int i=0;i<_maxBranches;i++){
		_sequence[i+1] = 0;
		_typeArray[i+1] = 0;
		_branchIndex[i] = 0;
		_branchCentre[i] = DTPoint(-1,-1);
	}

	memset(_sequenceString, 0, (_maxBranches+1)*5*sizeof(char));
}

FiducialData::~FiducialData(void)
{
}

void FiducialData::init( int *in_sequence )
{
	if( in_sequence[0] < _maxBranches ){
		memcpy(_sequence,in_sequence,(in_sequence[0]+1)*sizeof(int));
		memcpy(_typeArray,in_sequence,(in_sequence[0]+1)*sizeof(int));
	}else{
//		cout << "fiducial sequence lenght: " << in_sequence[0] << endl;
//		cout << "fiducial sequence too long for current settings!" << endl;
		assert(false);
	}
}

/*
DTPoint FiducialData::getBranchCentre(const int& i){
	assert( i>=0 && i<_sequence[0] );
	return _branchCentre[i];
}
*/

void FiducialData::setBranchCentre(const int& i, const DTPoint& p){
	assert( i>=0 && i<_sequence[0] );
	_branchCentre[i] = p;
}


DTPoint FiducialData::getCorner(const int& i){
	assert( i>=0 && i<4 );
	return _corner[i];
}

void FiducialData::setCorner(const int& i, const DTPoint& p){
	assert( i>=0 && i<4 );
	_corner[i] = p;
}

bool operator==(const FiducialData& a, const FiducialData& b){
	return (a.getCentre()==b.getCentre() && a.getType()==b.getType());
}//typedef FiducialDataList FiducialDataList;

int FiducialData::getBranchIndex(const int& i){ return _branchIndex[i]; }
void FiducialData::setBranchCentre(const int& i, const int& index){ _branchIndex[i] = index; }

//int * FiducialData::Sequence() { return _sequence; }

//int * FiducialData::getSequence() { return _sequence; }

#ifndef __SYMBIAN32__
char * FiducialData::getSequenceString( bool colorInfo ) {
	// WARNING! This assumes that the sequence does not change
	if( _sequenceString[0] != 0 ){
		return _sequenceString;
	}

	// TODO: the following was commented out
	// to  be consistent in sequence input and sequence output
	// i.e. the fact that sequences should NOT start with number of branches
	//sprintf(result,"%d",_sequence[0]);

	sprintf(_sequenceString,"%d",_sequence[1]);
	for(int i=2;i<=_sequence[0];i++){
		sprintf(_sequenceString,"%s,%d",_sequenceString,_sequence[i]);
	}
	if( colorInfo ){
		if( _whiteRoot ){
			sprintf(_sequenceString,"%s,w",_sequenceString);
		}else{
			sprintf(_sequenceString,"%s,b",_sequenceString);
		}
	}
	return _sequenceString;
}
#endif //ndef __SYMBIAN32__

int * FiducialData::parseSequenceL( const char * in_fiducialSequence, bool& white ){
	// decode the string, which is assumed to be in the form
	// "a[0], a[1], .., a[N-1]"
	char stringcopy[512] = {0};
	char *tmp = stringcopy;
	strcpy(tmp, in_fiducialSequence);

	// strip [, {, or (
	while( (*tmp == '[' || *tmp == '{' || *tmp == '(' || *tmp == ' ') && *tmp != 0 ){
		tmp++;
	}

	white = false;
	IntList * result = IntList::newL();
	for(char * tok = strtok( tmp, "," );tok!=NULL;tok = strtok( NULL, "," )){
		if( atoi(tok)==0 && strchr(tok,'0')==NULL ){
			if( strchr(tok,'w')!=NULL || strchr(tok,'W')!=NULL ){
				white = true;
			}
		}else{
			result->appendL( atoi(tok) );
		}
	}

	if( result->getSize() < 2 ){
		delete result;
		return NULL;
	}

	#ifndef __SYMBIAN32__
	int * toReturn = new int[result->getSize()+1];
	#else
	int * toReturn = new (ELeave) int[result->getSize()+1];
	#endif

	int i=0;
	toReturn[i++] = result->getSize();
	for(result->reset();!(result->isNull());result->fwd()){
		toReturn[i++] = result->getData();
	}
	delete result;

	return toReturn;
}


//int getRootColor(){ return rootColor; }
//void setRootColor(const int& in_rootColor){ rootColor = in_rootColor; }
//int& RootColor(){ return rootColor; }

