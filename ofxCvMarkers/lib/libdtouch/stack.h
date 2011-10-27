/***************************************************************************
                          stack.h  -  description
                             -------------------
    begin                : Fri Nov 28 2003
    copyright            : (C) 2003 by Enrico Costanza
    email                : e.costanza@ieee.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#ifndef EC_STACK
#define EC_STACK

#include "list.h"
#include "pool.h"

template <class T> class Stack
{
protected:
	ListItem<T> * _first;

	int _size;

public:
	// constuctors
	Stack() { _first = NULL; _size=0; }

	// destructor
	~Stack() {
		//delete _first; _first=NULL;
		ListItem<T> * current = _first;
		while( current != NULL ){
			ListItem<T> * tmp = current;
			current = current->getNext();
			delete tmp;
		}
		_first = NULL;
		_size=0;
	}

	// i/o methods

	int getSize(void) { return _size; }
	//int size(void) { return _size; }

	void pushL( const T &data ){
		#ifndef __SYMBIAN32__
		ListItem<T> * tmp = new ListItem<T>(data, _first);
		#else
		ListItem<T> * tmp = new (ELeave) ListItem<T>(data, _first);
		#endif
		_first = tmp;
		_size++;
	}

	T pop(){
		T result = _first->getData();
		ListItem<T> *tmp = _first;
		_first = _first->getNext();
		tmp->setNext( NULL );
		delete tmp;
		_size--;
		return result;
	}

	bool isEmpty( void ){ return( _first == NULL ); }

	ListItem<T> *getPtr() { return _first; }

};


//*
template <class T>
class PDStack : public Stack<T>
{
protected:
	using Stack<T>::_first;
	using Stack<T>::_size;
	DynamicPool<T> * _pool;

	//PDStack() { }
	// constuctors
	PDStack() : Stack<T>() { }

	void constructL(const int &in_poolSize){
		_pool = DynamicPool<T>::newL(in_poolSize);
	}

public:
	static PDStack * newL(const int &in_poolSize=1000){
		#ifndef __SYMBIAN32__
		PDStack * self = new PDStack();
		self->constructL(in_poolSize);
		#else
		PDStack * self = new (ELeave) PDStack();
		CleanupStack::PushL(self);
		self->constructL(in_poolSize);
		CleanupStack::Pop(self);
		#endif
		return self;
	}

	// destructor
	virtual ~PDStack() {
		ListItem<T> *current = _first;
		while( current != NULL ){
			ListItem<T> *tmp = current;
			current = current->getNext();
			delete tmp;
		}
		_first = NULL;
		_size=0;
		delete _pool;
	}

	void reset(){
		if( _first==NULL ){return;}
		ListItem<T> * current = _first;
		while( current->getNext() != NULL ){
			current = current->getNext();
		}
		_pool->pushList(_first,current,_size);
		_first = NULL;
		_size=0;
	}

	// i/o methods

	void pushL( const T &data ){
		ListItem<T> *tmp = _pool->getElementL();
		tmp->setData( data );
		tmp->setNext( _first );
		_first = tmp;
		_size++;
	}

	T pop(){
		T result = _first->getData();
		ListItem<T> *tmp = _first;
		_first = _first->getNext();
		_pool->pushElement( tmp );
		//tmp->next = NULL;
		//delete tmp;
		_size--;
		return result;
	}

	bool isEmpty( void ){ return( _first == NULL ); }

	ListItem<T> *getPtr() { return _first; }

};
//*/


#endif
