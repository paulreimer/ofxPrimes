/***************************************************************************
                          listhash.h  -  description
                             -------------------
    begin                : Mon Dec 8 2003
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
#ifndef EC_LISTHASH
#define EC_LISTHASH

#include "list.h"

#ifdef __SYMBIAN32__
#include <string.h>
//#include <e32base.h>
#endif

// TODO: if this is harmful, just rename List::check()
#ifdef check
#undef check
#endif

// Warning: this is defined as a template, but in practice it will
// work only for integers! (which is all I need now)
// To make it work in general it is needed to define a method that
// "hashes" T to int, but this requires partial specialization
// which is problematic in MS Windows (...)

template <class T>
class ListHash : public List<T>
{
protected:
	using List<T>::_first;
	using List<T>::_last;
	using List<T>::_current;
	using List<T>::_size;

	ListItem<T> **_toc;
	int _capacity;

	ListHash() {}

	// moveTo moves the current pointer to the first element in the List with value equal to target
	bool moveTo( const T target ){
		_current = _toc[target];
		return (_current != NULL);
	}

	// seek returns the pointer to the first element in the List with value equal to target
	ListItem<T> *seek( const T target ){
		return _toc[target];
	}

	ListHash( const int &initialSize ) :
		List<T>(),
		_capacity(initialSize)
	{ }

	void constructL(){
		//_toc = new (ListItem<T> *)[_capacity];
		_toc = new ListItem<T>*[_capacity];
		memset( _toc, 0, _capacity*sizeof(ListItem<T> *) );
	}

public:
	static ListHash * newL( const int &initialSize ){
		#ifndef __SYMBIAN32__
		ListHash * self = new ListHash(initialSize);
		self->constructL();
		#else
		ListHash * self = new (ELeave) ListHash(initialSize);
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}

	~ListHash(){
		//std::cerr << "~ListHash called" << std::endl;
		//delete _first; _first != NULL;
		_current = _first;
		while( _current != NULL ){
			ListItem<T> *tmp = _current;
			_current = _current->getNext();
			delete tmp;
		}

		_current=_last=_first=NULL; _size=0;
		delete [] _toc;
	}

	void empty() {
		// the following is efficient because the array
		// is very sparsely populated, so using a memset would
		// be very slow
		for(this->reset();!(this->nullTest());this->fwd()){
			_toc[this->getData()] = NULL;
		}

		// delete _first; _first=NULL;
		_current = _first;
		while( _current != NULL ){
			ListItem<T> *tmp = _current;
			_current = _current->getNext();
			delete tmp;
		}
		_current = _first = _last = NULL;
		_size=0;
	}

	void expandL(){
		//ListItem<T> **tmp = new (ListItem<T> *)[_capacity+_capacity];
		#ifndef __SYMBIAN32__
		ListItem<T> **tmp = new ListItem<T>*[_capacity+_capacity];
		#else
		ListItem<T> **tmp = new (ELeave) ListItem<T>*[_capacity+_capacity];
		#endif
		memcpy( tmp, _toc, _capacity*sizeof(ListItem<T> *) );
		memset( tmp + _capacity, 0, _capacity*sizeof(ListItem<T> *) );
		delete [] _toc;
		_toc = tmp;
		_capacity*=2;
	}

	// i/o methods

	void appendL( const T &in_data ){
		if( _toc[in_data] != NULL ){
			// data already in list
			// do nothing
			return;
		}
		if( _first == NULL ){
			#ifndef __SYMBIAN32__
			_first = new ListItem<T>(in_data);
			#else
			_first = new (ELeave) ListItem<T>(in_data);
			#endif
			// double link:
			_first->setPrev(NULL);
			_current = _first;
			_last = _first;
		}else{
			#ifndef __SYMBIAN32__
			_last->setNext( new ListItem<T>( in_data, NULL ) );
			#else
			_last->setNext( new (ELeave) ListItem<T>( in_data, NULL ) );
			#endif
			// double link:
			_last->getNext()->setPrev( _last );
			_last = _last->getNext();
		}
		_toc[in_data] = _last;
		_size++;
		return;
	}

	void append( ListHash<T> *src ){
		//cout << "Appending a ListHash to another" << endl;
		if( src->isEmpty() ){
			// do nothing
			return;
		}
		if( _first == NULL ){
			_first = src->_first;
			_current = _first;
			_last = src->_last;
			memcpy(_toc,src->_toc,_capacity*sizeof(ListItem<T> *));
		}
		else{
			_last->next = src->_first;
			// double link:
			_last->next->_prev = _last;
			_last = src->_last;
			if(src->_size > _size){
				for(ListItem<T> *tmp = _first;tmp!=NULL;tmp=tmp->next){
					src->_toc[tmp->data] = tmp;
				}
				memcpy(_toc,src->_toc,_capacity*sizeof(ListItem<T> *));
			}else{
				for(ListItem<T> *tmp = src->_first;tmp!=NULL;tmp=tmp->next){
					_toc[tmp->data] = tmp;
				}
			}
		}
		_size += src->_size;

		memset(src->_toc,NULL,src->_size);
		src->_size = 0;
		src->_first = NULL;
		src->_current = NULL;
		src->_last = NULL;
		return;
	}

	void remove( const T &data ){
		ListItem<T> *toBeDeleted = _toc[data];
		if( toBeDeleted == NULL ){
			return;
		}

		if( toBeDeleted->getPrev() == NULL ){
			_first = _first->getNext();
			if( _first != NULL ){
				_first->setPrev( NULL );
			}
			if( _last == toBeDeleted ){
				_last = _first;
			}
			toBeDeleted->setNext( NULL );
			delete toBeDeleted;
			_size--;
			_toc[data] = NULL;
			return;
		}

		toBeDeleted->getPrev()->setNext( toBeDeleted->getNext() );
		if( toBeDeleted->getNext() != NULL ){
			toBeDeleted->getNext()->setPrev( toBeDeleted->getPrev() );
		}
		if( _last == toBeDeleted ){
			_last = toBeDeleted->getPrev();
		}
		toBeDeleted->setNext( NULL );
		delete toBeDeleted;

		_toc[data] = NULL;
		_size--;
		return;
	}

	void replace( const T &data_in, const T &data_out ){
		if( _toc[data_out] != NULL &&  _toc[data_in] == NULL ){
			_toc[data_out]->setData( data_in );
			_toc[data_in] = _toc[data_out];
			_toc[data_out] = NULL;
		}
	}

	bool check( const T &target ) const {
		return (_toc[target] != NULL);
	}

	ListItem<T> **getToc(){
		return _toc;
	}
};




#endif
