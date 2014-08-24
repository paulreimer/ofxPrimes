/***************************************************************************
                          ragfactory.cpp  -  description
                             -------------------
    begin                : Wed Nov 26 2003
    copyright            : (C) 2003 by Enrico Costanza
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


/*  Changes

	Code optimization by Jorge M Santiago

*/

//
#include "ragbuilder.h"

/*
RAGBuilder::RAGBuilder( const unsigned char *in_image, const int &in_width, const int &in_height,
	const unsigned char *in_threshold, RegionAdjacencyGraph *in_rag, bool *in_stored )
	: _width(in_width), _height(in_height), _image(in_image),
		_threshold(in_threshold), _stored(in_stored), _rag(in_rag){
	// do nothing?

	//_stored = new bool[_width*_height];
	memset(_stored,0,_width*_height*sizeof(bool));
}

RAGBuilder::~RAGBuilder(){
	//delete [] _stored;
}
*/

/*
int RAGBuilder::buildRAGL( const int &maxRegions ){
	// first pixel
	_rag->_region[0]->appendL( DTPoint(0,0) );
	if( *_image > *_threshold ){
		_rag->_region[0]->_white = true;
	}else{
		_rag->_region[0]->_white = false;
	}
	*(_rag->_labelsMap) = _rag->getNewLabelL();
	_rag->_region[0]->appendToLabelPtrListL( *(_rag->_labelsMap) );

	int x;
	int y;

	const unsigned char *current;
	const unsigned char *above;
	const unsigned char *left;
	const unsigned char *aboveLeft;

	const unsigned char *currentThreshold;
	const unsigned char *aboveThreshold;
	const unsigned char *leftThreshold;
	const unsigned char *aboveLeftThreshold;

	int **currentLabel;
	int **aboveLabel;
	int **leftLabel;
	int **aboveLeftLabel;

	//first row
	{
		y=0;
		current = _image + 1;
		left = _image;
		currentThreshold = _threshold + 1;
		leftThreshold = _threshold;
		currentLabel = (_rag->_labelsMap)+1;
		leftLabel = _rag->_labelsMap;
		for(x=1;x<_width;x++){
			if( (*current >= *currentThreshold) == (*left >= *leftThreshold) ){
				// current is the same colour as left
				// set the label of the current pel to be the same as left
				// *currentLabel = *leftLabel;
				(_rag->_labelsMap)[x+y*_width] = (_rag->_labelsMap)[x+y*_width-1];
				// *currentLabel = (_rag->_labelsMap)[x+y*_width+1];
				// add current to the region of left
				_rag->_region[**currentLabel]->appendL( x, y );
			}else{
				// current is of a different colour from left
				// create a new label
				// *currentLabel = _rag->_used++;
				*currentLabel = _rag->getNewLabelL();
				_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
				// set the colour of the new region
				if( (*current >= *currentThreshold) ){
					_rag->_region[**currentLabel]->_white = true;
				}else{ _rag->_region[**currentLabel]->_white = false; }
				// add current to the new region
				_rag->_region[**currentLabel]->appendL( x, y );
				// link the new region to the one on the left
				_rag->linkL(**currentLabel,**leftLabel);
			}
			current++;
			left++;
			currentThreshold++;
			leftThreshold++;
			currentLabel++;
			leftLabel++;

			//std::cout << "(x,y): (" << x << "," << y << ")";
			//std::cout << " l: " << *leftLabel << " " ;
			//std::cout << ((*left>*leftThreshold)?("w"):("b")) << std::endl;
		}
	}

	for(y=1;y<_height;y++){
	// deal with the first pixel of the row
		x=0;
		current = _image+y*_width;
		above = _image+(y-1)*_width;
		currentThreshold = _threshold+y*_width;
		aboveThreshold = _threshold+(y-1)*_width;
		currentLabel = (_rag->_labelsMap)+y*_width;
		aboveLabel = (_rag->_labelsMap)+(y-1)*_width;
		if( (*current >= *currentThreshold) == (*above >= *aboveThreshold) ){
			// current is the same colour as above
			// set the label of the current pel to be the same as above
			*currentLabel = *aboveLabel;
			//(_rag->_labelsMap)[x+y*_width] = (_rag->_labelsMap)[x+(y-1)*_width];
			//if( (_rag->_labelsMap)[x+y*_width] != (_rag->_labelsMap)[x+(y-1)*_width] ){
			//	cout << "label copy failed!!!" << endl;
			//}
			// add current to the region of above
			_rag->_region[**currentLabel]->appendL( x, y );
		}else{
			// current is of a different colour from above
			// create a new label
			*currentLabel = _rag->getNewLabelL();
			_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
			// set the colour of the new region
			if( (*current >= *currentThreshold) ){
				_rag->_region[**currentLabel]->_white = true;
				// TODO: check whether the following should be 0 or **currentLabel
			}else{ _rag->_region[**currentLabel]->_white = true; }
			// add current to the new region
			_rag->_region[**currentLabel]->appendL( x, y );
			// link the new region to the one above
			_rag->linkL(**currentLabel,**aboveLabel);
		}

		current = _image+y*_width+1;
		above = _image+(y-1)*_width+1;
		left = _image+y*_width;
		aboveLeft = _image+(y-1)*_width;

		currentThreshold = _threshold+y*_width+1;
		aboveThreshold = _threshold+(y-1)*_width+1;
		leftThreshold = _threshold+y*_width;
		aboveLeftThreshold = _threshold+(y-1)*_width;

		currentLabel = (_rag->_labelsMap)+y*_width+1;
		aboveLabel = (_rag->_labelsMap)+(y-1)*_width+1;
		leftLabel = (_rag->_labelsMap)+y*_width;
		aboveLeftLabel = (_rag->_labelsMap)+(y-1)*_width;

		for(x=1;x<_width;x++){
			//if( (_rag->_region[*aboveLabel]->isEmpty()) || (_rag->_region[*leftLabel]->isEmpty()) ){
			//	std::cout << "Empty regions here!" << std::endl;
			//}

			bool currentEqualsLeft = (*current >= *currentThreshold) == (*left >= *leftThreshold);
			bool currentEqualsAbove = (*current >= *currentThreshold) == (*above >= *aboveThreshold);
			if( currentEqualsAbove && !currentEqualsLeft ){
				// set the label of the current pel to be the same as the one above
				*currentLabel = *aboveLabel;
				// add current to the region of the one above
				_rag->_region[**currentLabel]->appendL( x, y );
				// link the region containing current and the region containing the left neighbour
				// in the RAG if they are not already linked
				//if( !_rag->check(**currentLabel,**leftLabel) ){
					_rag->linkL(**currentLabel,**leftLabel);
				//}
			}else if( !currentEqualsAbove && currentEqualsLeft ){
				// set the label of the current pel to be the same as the one on the left
				*currentLabel = *leftLabel;
				// add current to the region of the pel on the left
				_rag->_region[**currentLabel]->appendL( x, y );
				// link the region containing current and the region containing the neighbour above
				// in the RAG if they are not already linked
				//if( !_rag->check(**currentLabel,**aboveLabel) ){
					_rag->linkL(**currentLabel,**aboveLabel);
				//}
			}else if( !currentEqualsAbove && !currentEqualsLeft ){
				// in this case current is different from both "direct" neighbours
				// if current is black, the neighbour above left needs to be checked
				if( *current < *currentThreshold ){
					// if the left above pel is black as well..
					if( *aboveLeft < *aboveLeftThreshold ){
						// .. the two are part of the same region
						// set the label of the current pel to be the same as the one above left
						*currentLabel = *aboveLeftLabel;
						// add current to the region of the pel above left
						_rag->_region[**currentLabel]->appendL( x, y );
					}else{
						// in this case current is black and the 3 neighbours are white
						// create a new label for current
						*currentLabel = _rag->getNewLabelL();
						_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
						// no need to set the colour of the new region
						// it is black, which is the default
						_rag->_region[**currentLabel]->_white = false;
						// add current to the new region
						_rag->_region[**currentLabel]->appendL( x, y );
						// link the new region to the one above (just need to link current to any of the neighbours)
						_rag->linkL(**currentLabel,**aboveLabel);
					}// end if..else (*aboveLeft < *aboveLeftThreshold)
				}else{ // current is white and the two "direct" neighbours are black
					// create a new white region for current
					*currentLabel = _rag->getNewLabelL();
					_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
					_rag->_region[**currentLabel]->_white = true;
					_rag->_region[**currentLabel]->appendL( x, y );
					// if the left and above neighbours are not in the same regions
					if( **leftLabel != **aboveLabel ){
						// merge the two regions
						_rag->mergeL(**aboveLabel,**leftLabel);
					}
					// link the new region to one of the two
					_rag->linkL(**currentLabel,**aboveLabel);
				}
			}else{ // current is the same colour as left and above
				if( **leftLabel != **aboveLabel ){
					// the pels left and above are in fact in the same region
					// merge the two regions
					//int r = _rag->mergeL(**aboveLabel,**leftLabel);
					_rag->mergeL(**aboveLabel,**leftLabel);
					//if( r == 1 ){
						*currentLabel = *aboveLabel;
					//}else{
					//	*currentLabel = *leftLabel;
					//}
				}else{
					// set the label of the current pel to be the same as the one above
					*currentLabel = *leftLabel;
				}
				// add current to the region containing the neighbours
				_rag->_region[**currentLabel]->appendL( x, y );
			}

			++current;
			++above;
			++left;
			++aboveLeft;

			++currentThreshold;
			++aboveThreshold;
			++leftThreshold;
			++aboveLeftThreshold;

			++currentLabel;
			++aboveLabel;
			++leftLabel;
			++aboveLeftLabel;

			// check the graph size
			// if it's above a threshold
			// abort the construction
			if( !(_rag->_used < maxRegions) ){
				return -1;
				// clean up?
			}
		}
		//std::cout << "(x,y): (" << "x" << "," << y << ")" << std::endl;
	}

	return 0;
}

//

int RAGBuilder::buildRAGBorderL( const int &maxRegions ){
	// first pixel
	_rag->_region[0]->appendL( DTPoint(0,0) );
	if( *_image > *_threshold ){
		_rag->_region[0]->_white = true;
	}else{ _rag->_region[0]->_white = false; }
	*(_rag->_labelsMap) = _rag->getNewLabelL();
	_rag->_region[0]->appendToLabelPtrListL( *(_rag->_labelsMap) );

	int x;
	int y;

	const unsigned char *current;
	const unsigned char *above;
	const unsigned char *left;
	const unsigned char *aboveLeft;

	const unsigned char *currentThreshold;
	const unsigned char *aboveThreshold;
	const unsigned char *leftThreshold;
	const unsigned char *aboveLeftThreshold;

	int **currentLabel;
	int **aboveLabel;
	int **leftLabel;
	int **aboveLeftLabel;

	//first row
	{
		y=0;
		current = _image + 1;
		left = _image;
		currentThreshold = _threshold + 1;
		leftThreshold = _threshold;
		currentLabel = (_rag->_labelsMap)+1;
		leftLabel = _rag->_labelsMap;
		for(x=1;x<_width;x++){
			if( (*current >= *currentThreshold) == (*left >= *leftThreshold) ){
				// current is the same colour as left
				// set the label of the current pel to be the same as left
				// *currentLabel = *leftLabel;
				(_rag->_labelsMap)[x+y*_width] = (_rag->_labelsMap)[x+y*_width-1];
				// *currentLabel = (_rag->_labelsMap)[x+y*_width+1];
				// add current to the region of left
				_rag->_region[**currentLabel]->appendL( x, y );
			}else{
				// current is of a different colour from left
				// create a new label
				// *currentLabel = _rag->_used++;
				*currentLabel = _rag->getNewLabelL();
				_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
				// set the colour of the new region
				if( (*current >= *currentThreshold) ){
					_rag->_region[**currentLabel]->_white = true;
				}else{ _rag->_region[**currentLabel]->_white = false; }
				// add current to the new region
				_rag->_region[**currentLabel]->appendL( x, y );
				// link the new region to the one on the left
				_rag->linkL(**currentLabel,**leftLabel);
			}
			current++;
			left++;
			currentThreshold++;
			leftThreshold++;
			currentLabel++;
			leftLabel++;

			//std::cout << "(x,y): (" << x << "," << y << ")";
			//std::cout << " l: " << *leftLabel << " " ;
			//std::cout << ((*left>*leftThreshold)?("w"):("b")) << std::endl;
		}
	}

	for(y=1;y<_height;y++){
	// deal with the first pixel of the row
		x=0;
		current = _image+y*_width;
		above = _image+(y-1)*_width;
		currentThreshold = _threshold+y*_width;
		aboveThreshold = _threshold+(y-1)*_width;
		currentLabel = (_rag->_labelsMap)+y*_width;
		aboveLabel = (_rag->_labelsMap)+(y-1)*_width;
		if( (*current >= *currentThreshold) == (*above >= *aboveThreshold) ){
			// current is the same colour as above
			// set the label of the current pel to be the same as above
			*currentLabel = *aboveLabel;
			//(_rag->_labelsMap)[x+y*_width] = (_rag->_labelsMap)[x+(y-1)*_width];
			//if( (_rag->_labelsMap)[x+y*_width] != (_rag->_labelsMap)[x+(y-1)*_width] ){
			//	cout << "label copy failed!!!" << endl;
			//}
			// add current to the region of above
			_rag->_region[**currentLabel]->appendL( x, y );
		}else{
			// current is of a different colour from above
			// create a new label
			*currentLabel = _rag->getNewLabelL();
			_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
			// set the colour of the new region
			if( (*current >= *currentThreshold) ){
				_rag->_region[**currentLabel]->_white = true;
				// TODO: check whether the following should be 0 of **currentLabel
			}else{ _rag->_region[**currentLabel]->_white = true; }
			// add current to the new region
			_rag->_region[**currentLabel]->appendL( x, y );
			// link the new region to the one above
			_rag->linkL(**currentLabel,**aboveLabel);
		}

		current = _image+y*_width+1;
		above = _image+(y-1)*_width+1;
		left = _image+y*_width;
		aboveLeft = _image+(y-1)*_width;

		currentThreshold = _threshold+y*_width+1;
		aboveThreshold = _threshold+(y-1)*_width+1;
		leftThreshold = _threshold+y*_width;
		aboveLeftThreshold = _threshold+(y-1)*_width;

		currentLabel = (_rag->_labelsMap)+y*_width+1;
		aboveLabel = (_rag->_labelsMap)+(y-1)*_width+1;
		leftLabel = (_rag->_labelsMap)+y*_width;
		aboveLeftLabel = (_rag->_labelsMap)+(y-1)*_width;

		for(x=1;x<_width;x++){
			//if( (_rag->_region[*aboveLabel]->isEmpty()) || (_rag->_region[*leftLabel]->isEmpty()) ){
			//	std::cout << "Empty regions here!" << std::endl;
			//}

			bool currentEqualsLeft = (*current >= *currentThreshold) == (*left >= *leftThreshold);
			bool currentEqualsAbove = (*current >= *currentThreshold) == (*above >= *aboveThreshold);
			if( currentEqualsAbove && !currentEqualsLeft ){
				// set the label of the current pel to be the same as the one above
				*currentLabel = *aboveLabel;
				// add current to the region of the one above
				_rag->_region[**currentLabel]->appendL( x, y );
				// link the region containing current and the region
				// containing the left neighbour in the RAG
				_rag->linkL(**currentLabel,**leftLabel);
			}else if( !currentEqualsAbove && currentEqualsLeft ){
				// set the label of the current pel to be the same as the one on the left
				*currentLabel = *leftLabel;
				// add current to the region of the pel on the left
				_rag->_region[**currentLabel]->appendL( x, y );
				// link the region containing current and the region
				// containing the neighbour above in the RAG
				_rag->linkL(**currentLabel,**aboveLabel);
			}else if( !currentEqualsAbove && !currentEqualsLeft ){
				// in this case current is different from both "direct" neighbours
				// if current is black, the neighbour above left needs to be checked
				if( *current < *currentThreshold ){
					// if the left above pel is black as well..
					if( *aboveLeft < *aboveLeftThreshold ){
						// .. the two are part of the same region
						// set the label of the current pel to be the same as the one above left
						*currentLabel = *aboveLeftLabel;
						// add current to the region of the pel above left
						_rag->_region[**currentLabel]->appendL( x, y );
					}else{
						// in this case current is black and the 3 neighbours are white
						// create a new label for current
						*currentLabel = _rag->getNewLabelL();
						_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
						// no need to set the colour of the new region
						// it is black, which is the default
						_rag->_region[**currentLabel]->_white = false;
						// add current to the new region
						_rag->_region[**currentLabel]->appendL( x, y );
						// link the new region to the one above (just need to link current to any of the neighbours)
						_rag->linkL(**currentLabel,**aboveLabel);
					}// end if..else (*aboveLeft < *aboveLeftThreshold)
				}else{ // current is white and the two "direct" neighbours are black
					// create a new white region for current
					*currentLabel = _rag->getNewLabelL();
					_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
					_rag->_region[**currentLabel]->_white = true;
					_rag->_region[**currentLabel]->appendL( x, y );
					// if the left and above neighbours are not in the same regions
					if( **leftLabel != **aboveLabel ){
						// merge the two regions
						_rag->mergeL(**aboveLabel,**leftLabel);
					}
					// link the new region to one of the two
					_rag->linkL(**currentLabel,**aboveLabel);
				}
			}else{ // current is the same colour as left and above
				if( **leftLabel != **aboveLabel ){
					// the pels left and above are in fact in the same region
					// merge the two regions
					//int r = _rag->mergeL(**aboveLabel,**leftLabel);
					_rag->mergeL(**aboveLabel,**leftLabel);
					*currentLabel = *aboveLabel;
				}else{
					// set the label of the current pel to be the same as the one above
					*currentLabel = *leftLabel;
				}
				// add current to the region containing the neighbours
				//_rag->_region[**currentLabel]->appendL( x, y );
			}

			++current;
			++above;
			++left;
			++aboveLeft;

			++currentThreshold;
			++aboveThreshold;
			++leftThreshold;
			++aboveLeftThreshold;

			++currentLabel;
			++aboveLabel;
			++leftLabel;
			++aboveLeftLabel;

			// check the graph size
			// if it's above a threshold
			// abort the construction
			if( !(_rag->_used < maxRegions) ){
				return -1;
			}
		}
		//std::cout << "(x,y): (" << "x" << "," << y << ")" << std::endl;

	}

  return 0;
}
*/

int RAGBuilder::buildRAGFullBorderL( const unsigned char *_image,
		const unsigned char *_threshold, RegionAdjacencyGraph *_rag )
{
	const int _width = _rag->getWidth();
	const int _height = _rag->getHeight();

	bool *_stored = _rag->getStored();

	const int maxRegions = _rag->getSize() - 2;

	// first pixel
	_rag->_region[0]->appendL( 0, 0 );
	if( *_image > *_threshold ){
		_rag->_region[0]->_white = true;
	}else{
		_rag->_region[0]->_white = false;
	}
	*(_rag->_labelsMap) = _rag->getNewLabelL();
	// store the pointer to the current label in the current region
	// so that it can be changed if the region will be merged
	_rag->_region[0]->appendToLabelPtrListL( *(_rag->_labelsMap) );

	//bool *storedPtr = _stored;
	//int limit = _height*_width;
	//for(int i=0;i<limit;i++){
	//	*storedPtr++ = false;
	//}

	// !!! JMS
	memset(_stored, false, _height*_width*sizeof(unsigned char));

	*_stored = true;

	int x;
	int y;

	const unsigned char *current;
	const unsigned char *above;
	const unsigned char *left;
	const unsigned char *aboveLeft;

	const unsigned char *currentThreshold;
	const unsigned char *aboveThreshold;
	const unsigned char *leftThreshold;
	const unsigned char *aboveLeftThreshold;

	int **currentLabel;
	int **aboveLabel;
	int **leftLabel;
	int **aboveLeftLabel;

	bool *currentStored;
	bool *aboveStored;
	bool *leftStored;

	//first row
	{
		y=0;
		current = _image + 1;
		left = _image;
		currentThreshold = _threshold + 1;
		leftThreshold = _threshold;
		currentLabel = (_rag->_labelsMap)+1;
		leftLabel = _rag->_labelsMap;

		currentStored = _stored + 1;
		leftStored = _stored;

		for(x=1;x<_width;x++){
			if( (*current >= *currentThreshold) == (*left >= *leftThreshold) ){
				// current is the same colour as left
				// set the label of the current pel to be the same as left
				//*currentLabel = *leftLabel;
				(_rag->_labelsMap)[x+y*_width] = (_rag->_labelsMap)[x+y*_width-1];
				//*currentLabel = (_rag->_labelsMap)[x+y*_width+1];
				// add current to the region of left
				_rag->_region[**currentLabel]->appendL( x, y );
			}else{
				// current is of a different colour from left
				// create a new label
				//*currentLabel = _rag->_used++;
				*currentLabel = _rag->getNewLabelL();
				// store the pointer to the current label in the current region
				// so that it can be changed if the region will be merged
				_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
				// set the colour of the new region
				if( (*current >= *currentThreshold) ){
					_rag->_region[**currentLabel]->_white = true;
				}else{ _rag->_region[**currentLabel]->_white = false; }
				// add current to the new region
				_rag->_region[**currentLabel]->appendL( x, y );
				// link the new region to the one on the left
				_rag->linkL(**currentLabel,**leftLabel);
			}
			current++;
			left++;
			currentThreshold++;
			leftThreshold++;
			currentLabel++;
			leftLabel++;

			*currentStored++ = true;

			//std::cout << "(x,y): (" << x << "," << y << ")";
			//std::cout << " l: " << *leftLabel << " " ;
			//std::cout << ((*left>*leftThreshold)?("w"):("b")) << std::endl;
		}
	}

	for(y=1;y<_height;y++){
		// deal with the first pixel of the row
		x=0;

		int yOffset = y*_width;

		current = _image+yOffset;
		above = _image+yOffset-_width;

		currentThreshold = _threshold+yOffset;
		aboveThreshold = _threshold+yOffset-_width;

		currentLabel = (_rag->_labelsMap)+yOffset;
		aboveLabel = (_rag->_labelsMap)+yOffset-_width;

		*(_stored+yOffset) = true;

		if( (*current >= *currentThreshold) == (*above >= *aboveThreshold) ){
			// current is the same colour as above
			// set the label of the current pel to be the same as above
			*currentLabel = *aboveLabel;
			//(_rag->_labelsMap)[x+yOffset] = (_rag->_labelsMap)[x+yOffset-_width];
			//if( (_rag->_labelsMap)[x+yOffset] != (_rag->_labelsMap)[x+yOffset-_width] ){
			//	cout << "label copy failed!!!" << endl;
			//}
			// add current to the region of above
			_rag->_region[**currentLabel]->appendL( x, y );
		}else{
			// current is of a different colour from above
			// create a new label
			*currentLabel = _rag->getNewLabelL();
			// store the pointer to the current label in the current region
			// so that it can be changed if the region will be merged
			_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
			// set the colour of the new region
			if( (*current >= *currentThreshold) ){
				_rag->_region[**currentLabel]->_white = true;
				//TODO: check wether the following is correct
			}else{ _rag->_region[**currentLabel]->_white = true; }
			// add current to the new region
			_rag->_region[**currentLabel]->appendL( x, y );
			// link the new region to the one above
			_rag->linkL(**currentLabel,**aboveLabel);
		}

		current = _image+yOffset+1;
		above = _image+yOffset-_width+1;
		left = _image+yOffset;
		aboveLeft = _image+yOffset-_width;

		currentThreshold = _threshold+yOffset+1;
		aboveThreshold = _threshold+yOffset-_width+1;
		leftThreshold = _threshold+yOffset;
		aboveLeftThreshold = _threshold+yOffset-_width;

		currentLabel = (_rag->_labelsMap)+yOffset+1;
		aboveLabel = (_rag->_labelsMap)+yOffset-_width+1;
		leftLabel = (_rag->_labelsMap)+yOffset;
		aboveLeftLabel = (_rag->_labelsMap)+yOffset-_width;

		currentStored = _stored+yOffset+1;
		aboveStored = _stored+yOffset-_width+1;
		leftStored = _stored+yOffset;

		for(x=1;x<_width;x++){
			bool currentEqualsLeft = (*current >= *currentThreshold) == (*left >= *leftThreshold);
			bool currentEqualsAbove = (*current >= *currentThreshold) == (*above >= *aboveThreshold);
			if( currentEqualsAbove && !currentEqualsLeft ){
				// set the label of the current pel to be the same as the one above
				*currentLabel = *aboveLabel;
				// add current to the region of the one above
				_rag->_region[**currentLabel]->appendL( x, y );
				// mark the current pixel as stored
				*currentStored = true;
				// link the region containing current and the region
				// containing the left neighbour in the RAG
				_rag->linkL(**currentLabel,**leftLabel);

				if( !(*leftStored) ){
  					// add the left pixel to the border of the region to the left
  					_rag->_region[**leftLabel]->appendL( x-1, y );
					// mark the left pixel as stored
					*leftStored = true;
				}
			}else if( !currentEqualsAbove && currentEqualsLeft ){
				// set the label of the current pel to be the same as the one on the left
				*currentLabel = *leftLabel;
				// add current to the region of the pel on the left
				_rag->_region[**currentLabel]->appendL( x, y );
				// mark the current pixel as stored
				*currentStored = true;
				// link the region containing current and the region
				// containing the neighbour above in the RAG
				_rag->linkL(**currentLabel,**aboveLabel);

				if( !(*aboveStored) ){
					// add the pixel above to the border of the region above
					_rag->_region[**aboveLabel]->appendL( x, y-1 );
					// mark the pixel above as stored
					*aboveStored = true;
				}
			}else if( !currentEqualsAbove && !currentEqualsLeft ){
				// in this case current is different from both "direct" neighbours
				// if current is black, the neighbour above left needs to be checked
				if( *current < *currentThreshold ){
					// if the left above pel is black as well..
					if( *aboveLeft < *aboveLeftThreshold ){
						// .. the two are part of the same region
						// set the label of the current pel to be the same as the one above left
						*currentLabel = *aboveLeftLabel;
						// add current to the region of the pel above left
						_rag->_region[**currentLabel]->appendL( x, y );
						// mark the current pixel as stored
						*currentStored = true;

						if( !(*leftStored) ){
							// add the pixel to the left to the border of the left region
							_rag->_region[**leftLabel]->appendL( x-1, y );
							// mark the left pixel as stored
							*leftStored = true;
						}
						if( !(*aboveStored) ){
    						// add the pixel above to the border of the left above
    						_rag->_region[**aboveLabel]->appendL( x, y-1 );
							// mark the pixel above as stored
							*aboveStored = true;
						}
					}else{
						// in this case current is black and the 3 neighbours are white
						// create a new label for current
						*currentLabel = _rag->getNewLabelL();
						// store the pointer to the current label in the current region
						// so that it can be changed if the region will be merged
						_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
						// no need to set the colour of the new region
						// it is black, which is the default
						_rag->_region[**currentLabel]->_white = false;
						// add current to the new region
						_rag->_region[**currentLabel]->appendL( x, y );
						// mark the current pixel as stored
						*currentStored = true;
						// link the new region to the one above (just need to link current to any of the neighbours)
						_rag->linkL(**currentLabel,**aboveLabel);

						if( !(*leftStored) ){
    						// add the pixel to the left to the border of the left region
    						_rag->_region[**leftLabel]->appendL( x-1, y );
							// mark the left pixel as stored
							*leftStored = true;
						}
						if( !(*aboveStored) ){
    						// add the pixel above to the border of the left above
    						_rag->_region[**aboveLabel]->appendL( x,y-1 );
							// mark the pixel above as stored
							*aboveStored = true;
						}
					}// end if..else (*aboveLeft < *aboveLeftThreshold)
				}else{ // current is white and the two "direct" neighbours are black
					// TODO: use 4-connectivity or 8-connectivity?
					// create a new white region for current
					*currentLabel = _rag->getNewLabelL();
					// store the pointer to the current label in the current region
					// so that it can be changed if the region will be merged
					_rag->_region[**currentLabel]->appendToLabelPtrListL( *currentLabel );
					// set the colour of the new region
					_rag->_region[**currentLabel]->_white = true;
					// append the current pixel to the new region
					_rag->_region[**currentLabel]->appendL( x, y );
					// mark the current pixel as stored
					*currentStored = true;
					// if the left and above neighbours are not in the same regions
					if( **leftLabel != **aboveLabel ){
						// merge the two regions
						//std::cout << "merge @ " << x << ", " << y << std::endl;
						_rag->mergeL(**aboveLabel,**leftLabel);
					}
					// link the new region to one of the two
					_rag->linkL(**currentLabel,**aboveLabel);

					if( !(*leftStored) ){
  						// add the pixel to the left to the border of the left region
  						_rag->_region[**leftLabel]->appendL( x-1, y );
						// mark the left pixel as stored
						*leftStored = true;
					}
					if( !(*aboveStored) ){
  						// add the pixel above to the border of the left above
  						_rag->_region[**aboveLabel]->appendL( x, y-1 );
						// mark the pixel above as stored
						*aboveStored = true;
					}
				}
			}else{ // current is the same colour as left and above
				if( **leftLabel != **aboveLabel ){
					// the pels left and above are in fact in the same region
					// merge the two regions
					//int r = _rag->mergeL(**aboveLabel,**leftLabel);
					//std::cout << "merge @ " << x << ", " << y << std::endl;
					_rag->mergeL(**aboveLabel,**leftLabel);
					*currentLabel = *aboveLabel;
				}else{
					// set the label of the current pel to be the same as the one above
					*currentLabel = *leftLabel;
				}
				// add current to the region containing the neighbours
				//_rag->_region[**currentLabel]->appendL( x, y );
			}

			++current;
			++above;
			++left;
			++aboveLeft;

			++currentThreshold;
			++aboveThreshold;
			++leftThreshold;
			++aboveLeftThreshold;

			++currentLabel;
			++aboveLabel;
			++leftLabel;
			++aboveLeftLabel;

			++currentStored;
			++aboveStored;
			++leftStored;

			// check the graph size
			// if it's above a threshold
			// abort the construction
			if( !(_rag->_used < maxRegions) ){
				return -1;
			}
		}
	}

	return 0;
}


