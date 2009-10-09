/***************************************************************************
          intlist.cpp  -
                             -------------------
    begin                : Thu Mar 1 2007
    copyright            : (C) 2007 by Enrico Costanza @ EPFL
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


#include  "intlist.h"

#include <assert.h>

int * IntList2ArrayL( IntList * l ){
	int size = l->getSize();
#ifndef __SYMBIAN32__
	int * result = new int[size+1];
	#else
	int * result = new (ELeave) int[size+1];
	#endif
	*result = size;
	int * rPtr = result+1;
	for(l->reset();!(l->nullTest()); *rPtr++ = l->fwdGet()){}
	//for(reset();!nullTest();fwd()){
	//	*rPtr++ = getData();
	//}
	return result;
}

IntList * array2IntList( int * data ){
	int size = data[0];
	IntList * result = IntList::newL();
	int * iPtr = data+1;
	for(int i=0;i<size;i++){
		result->appendL( *iPtr++ );
	}
	return result;
}

void sortIntList( IntList * l ){
	// bubble-sort the list in place
	// TODO: this could be optimized..
	bool swapped;
	do{
		swapped = false;
		for(l->reset();!(l->fwdNullTest());l->fwd()){
			if( l->getData() > l->getCurrent()->getNext()->getData() ){
				// swap the values (NOT THE NODES!)
				int tmp = l->getData();
				l->getCurrent()->setData( l->getCurrent()->getNext()->getData() );
				l->getCurrent()->getNext()->setData( tmp );
				swapped = true;
			}
		}
	}while(swapped);
}

void sortIntList( IntList * l, IntList * follower ){
	assert( l->getSize() == follower->getSize() );
	// bubble-sort the list
	// TODO: this could be optimized..
	bool swapped;
	do{
		swapped = false;
		for(l->reset(), follower->reset();
			!(l->fwdNullTest());
			l->fwd(), follower->fwd() ){
			if( l->getData() > l->getCurrent()->getNext()->getData() ){
				// swap the values (NOT THE NODES!)
				int tmp = l->getData();
				l->getCurrent()->setData( l->getCurrent()->getNext()->getData() );
				l->getCurrent()->getNext()->setData( tmp );
				swapped = true;

				tmp = follower->getData();
				follower->getCurrent()->setData( follower->getCurrent()->getNext()->getData() );
				follower->getCurrent()->getNext()->setData( tmp );
			}
		}
	}while(swapped);

	return;
}

/*
int intcmp(const void * a,const void * b){
	return ( *((int*) a) - *((int*)b) );
}
*/

int intcmp(const void *x, const void *y)
{
   if( *((int *)x) < *((int *)y) ) return -1;
   else if( *((int *)x) == *((int *)y) ) return 0;
   else
      return 1;
}

bool compareArrays( int * a, int * b ){
	// sort arrays so they are in the same order
	qsort( (void *)(a+1), a[0], sizeof(int), intcmp );
	qsort( (void *)(b+1), b[0], sizeof(int), intcmp );
	for(int i=0;i<=a[0];i++){
		if( a[i] != b[i] ){
			return false;
		}
	}
	return true;
}

bool arrayContains( int * a, int t ){
	for(int i=0;i<=a[0];i++){
		if( a[i] == t ){
			return true;
		}
	}
	return false;
}

bool compareIntList( IntList * a, IntList * b ){
	//sortIntList(a);
	//sortIntList(b);
	int * aa = IntList2ArrayL(a);
	int * ab = IntList2ArrayL(b);
	bool result = compareArrays(aa,ab);
	delete [] aa;
	delete [] ab;
	return result;
}

