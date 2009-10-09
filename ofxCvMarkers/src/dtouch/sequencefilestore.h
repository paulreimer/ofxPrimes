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

#ifndef _EC_SEQUENCESTORE
#define _EC_SEQUENCESTORE

#include "binarytree.h"
#include "fiducialdata.h"

class SequenceFileStore : public SequenceStore{
protected:
	SequenceFileStore() {}
	
	void loadSeqFromFile( const char * filename );

	BinaryTree<int *> * _storeTree;
public:
	SequenceFileStore( const char * filename );
	~SequenceFileStore();

	void addSequence( int * seq, bool white );
	bool contains(const int * seq, const bool white);
};

#endif
