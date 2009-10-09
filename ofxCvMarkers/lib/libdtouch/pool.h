/***************************************************************************
                          pool.h  -  description
                             -------------------
    begin                : Tue Nov 25 2003
    copyright            : (C) 2003 by Enrico Costanza
    email                : enrico@media.mit.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef EC_POOL
#define EC_POOL

#include "list.h"
//#include <assert.h>

#ifdef __SYMBIAN32__
#include <flogger.h>
//_LIT(KLogFileDir, "debug"); // logs to c:\logs\debug
//_LIT(KLogFileName, "debug.txt");
#endif //__SYMBIAN32__


template <class T>
class Pool
{
protected:
	ListItem<T> *_first;
	int _size;

	Pool() {}

	// constuctor
	Pool( const int &in_size ) : _size(in_size) {}

	void constructL(void){
		#ifndef __SYMBIAN32__
		ListItem<T> * ptr = new ListItem<T>;
		_first = ptr;
		for(int i=1; i<_size; i++){
			ptr->setNext( new ListItem<T> );
			ptr = ptr->getNext();
		}
		#else
		ListItem<T> * ptr = new (ELeave) ListItem<T>;
		_first = ptr;
		for(int i=1; i<_size; i++){
			ptr->setNext( new (ELeave) ListItem<T> );
			ptr = ptr->getNext();
		}
		#endif
	}

public:

	// destructor
	virtual ~Pool() {
		//delete _first; _first=NULL;
		ListItem<T> *current = _first;
		//_size;
		while( current != NULL ){
			ListItem<T> *tmp = current;
			current = current->getNext();
			delete tmp;
			_size--;
		}
		_first = NULL;
		_size = 0;
	}

	virtual ListItem<T> *getElementL() = 0;
	virtual void pushElement( ListItem<T> *element ) = 0;
	virtual void pushList( ListItem<T> *firstElement, ListItem<T> *lastElement, const int& in_size ) = 0;
};


template <class T>
class StaticPool : public Pool<T>
{
private:
	using Pool<T>::_first;
	using Pool<T>::_size;

	//ListItem<T> *_getElementResult;
	StaticPool( const int &in_size ) : Pool<T>(in_size) { }

public:
	static StaticPool * newL( const int &in_size ){
		#ifndef __SYMBIAN32__
		StaticPool * self = new StaticPool( in_size );
		self->constructL();
		#else
		StaticPool * self = new (ELeave) StaticPool( in_size );
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}

	virtual ~StaticPool(){ }

	ListItem<T> *getElementL(){
		ListItem<T> *result = _first;
		_first = _first->getNext();
		result->setNext( NULL );
		// TODO: keep track of size and throw exception if we go below 0?
		#ifdef _DEBUG
		_size--;
		#endif
		return result;
	}

	void pushElement( ListItem<T> *element ){
		element->setNext( _first );
		_first = element;
		#ifdef _DEBUG
		_size++;
		#endif
	}

	void pushList( ListItem<T> *firstElement, ListItem<T> *lastElement, const int& in_size ){
		lastElement->setNext( _first );
		_first = firstElement;
		#ifdef _DEBUG
		_size+=in_size;
		#endif
	}
};

template <class T>
class DynamicPool : public Pool<T>
{
protected:
	using Pool<T>::_first;
	using Pool<T>::_size;

	int _available;
	DynamicPool( const int &in_size) : Pool<T>(in_size), _available(in_size) { }

public:
	static DynamicPool<T> * newL( const int &in_size ){
		#ifndef __SYMBIAN32__
		DynamicPool * self = new DynamicPool<T>( in_size );
		self->constructL();
		#else
		DynamicPool * self = new (ELeave) DynamicPool<T>( in_size );
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}

	virtual ~DynamicPool() { }

	ListItem<T> * getElementL(){
		if( _available < 1 ){
			#ifndef __SYMBIAN32__
			this->constructL();
			#else
			//RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("expanding DynamicPool") );
			this->constructL();
			//RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("DynamicPool::constructL returned") );
			#endif //__SYMBIAN32__
 			_available = _size;
			_size *= 2;
		}
		ListItem<T> *result = _first;
		_first = _first->getNext();
		_available--;
		result->setNext( NULL );
		return result;
	}

	void pushElement( ListItem<T> *element ){
		element->setNext( _first );
		_first = element;
		_available++;
	}

	void pushList( ListItem<T> *firstElement, ListItem<T> *lastElement, const int& in_size ){
		lastElement->setNext( _first );
		_first = firstElement;
		_available+=in_size;
	}

};


#endif

