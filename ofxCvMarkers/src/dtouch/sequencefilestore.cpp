/***************************************************************************
	sequencefilestore.cpp

	Implmentation of the SequenceStore interface defined in fiducialdata.h
	(in dtouch core). This class loads the sequence data from a file, and
	it stores it in binary tree.

                             -------------------
    begin                : Wed Nov 15 2007
    copyright            : (C) 2007 by Enrico Costanza @ EPFL LDM
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

#include "sequencefilestore.h"

#include "fiducialdata.h"

#include <iostream>
#include <fstream>
#include <string>

using std::ifstream;

void SequenceFileStore::loadSeqFromFile( const char * filename ){
	ifstream * file = new ifstream(filename);
	if( ! (file->is_open()) ){
		cout << "cannot open " << filename << endl;
		// try to look for the file in a few folders up
		char newName[256] = "../../../";
		strcat( newName, filename );
		cout << "trying to open: " << newName << endl;
		delete file;
		file = new ifstream(newName);
	}else{
		cout << filename << " open" << endl;
	}
	if( file->is_open() ){
		std::string line;
		int counter = 0;
		while( std::getline(*file,line) ){
			if( line.find_first_of("//") == 0 ){
				continue;
			}
			bool currentWhite;
			int * current = FiducialData::parseSequenceL( line.c_str(), currentWhite );
			if( current != NULL ){
				addSequence( current, currentWhite );
				counter++;
			}
		}
		if( ! (*file).eof() ) {
			if( line.find_first_of("//") != 0 ){
				bool currentWhite;
				int * current = FiducialData::parseSequenceL( line.c_str(), currentWhite );
				if( current != NULL ){
					addSequence( current, currentWhite );
					counter++;
				}
			}
		}
		cout << counter << " sequences loaded" << endl;
	}else{
		cout << "*** *** ***" << endl;
		cout << "WARNING: cannot open " << filename << endl;
		cout << "*** *** ***" << endl;
	}
	delete file;
	return;
}

SequenceFileStore::SequenceFileStore( const char * filename )
{
	_storeTree = new BinaryTree<int *>();
	loadSeqFromFile( filename );
}

void delSeq( int * toDelete ){
	delete [] toDelete;
}

SequenceFileStore::~SequenceFileStore(){
	_storeTree->apply( delSeq );
	delete _storeTree;
}


void SequenceFileStore::addSequence( int * seq, bool white ){
	// convert the input sequence into a sequence that has last item
	// indicating the colour
	int * toAdd = new int[seq[0]+2];
	memcpy( toAdd, seq, (seq[0]+1)*sizeof(int) );
	if( white ){
		toAdd[toAdd[0]+1] = 0x1;
	}else{
		toAdd[toAdd[0]+1] = 0x2;
	}
	_storeTree->insert( toAdd );
	delete [] seq;
	return;
}

bool SequenceFileStore::contains(const int * seq, const bool white){
	//cout << "contains called" << endl;
	//for(int i=0;i<=seq[0];i++){
	//	cout << seq[i] << " ";
	//}cout << endl;
	int * toCheck = new int[seq[0]+2];
	memcpy( toCheck, seq, (seq[0]+1)*sizeof(int) );
	if( white ){
		toCheck[toCheck[0]+1] = 0x1;
	}else{
		toCheck[toCheck[0]+1] = 0x2;
	}
	bool result = _storeTree->contains(toCheck);
	delete [] toCheck;
	return result;
}


// end of file
