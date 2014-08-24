/***************************************************************************
                          listpool.h  -  description
                             -------------------
    begin                : Tue Nov 25 2003
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

#ifndef EC_LISTPOOL
#define EC_LISTPOOL

#include "list.h"
#include "pool.h"

template <class T>
class ListPool : public List<T>
{
protected:
	using List<T>::_first;
	using List<T>::_last;
	using List<T>::_current;
	using List<T>::_size;

	Pool<T> *_pool;

	ListPool() {}
public:
	// constuctors
	ListPool( Pool<T> *in_pool ) : List<T>(), _pool(in_pool) { }

	~ListPool(){
		if( _first != NULL ){
			_pool->pushList(_first, _last,_size);
			_first = NULL;
			_size = 0;
		}
	}

	void empty(){
		if( _first != NULL ){
			_pool->pushList(_first, _last,_size);
			_first = _last = _current = NULL;
			_size = 0;
		}
	}

	// i/o methods

	void appendL( const T &in_data ){
		if( _first == NULL ){
			_first = _pool->getElementL();
			_first->setData( in_data );
			//_current = _first;
			_last = _first;
		}
		else{
			_last->setNext( _pool->getElementL() );
			_last->getNext()->setData( in_data );

			//if( _last->next == NULL ){
			//	throw( "\n\nList<T>::append: allocation failed!\n\n" );
			//}

			_last = _last->getNext();
			// not too sure about the following
			//current = last;
		}
		_size++;
		return;
	}

	void append( ListPool<T> *src ){
		if( src->isEmpty() ){
			// do nothing
			return;
		}
		if( _first == NULL ){
			_first = src->_first;
			_current = _first;
			_last = src->_last;

		}
		else{
			_last->setNext( src->_first );
			_last = src->_last;
			// not too sure about the following
			//_current = _last;
		}
		_size += src->_size;

		src->_size = 0;
		src->_first = NULL;
		src->_current = NULL;
		src->_last = NULL;
		return;
	}

};



template <class T>
class ListPoolStatic : public List<T>
{
protected:
	using List<T>::_first;
	using List<T>::_last;
	using List<T>::_current;
	using List<T>::_size;

	StaticPool<T> _pool;

	ListPoolStatic() {}
public:
	// constuctors
	ListPoolStatic( int in_size ) : List<T>(), _pool(in_size) {  }

	// i/o methods

	void append( const T &in_data ){
		if( _first == NULL ){
			_first = _pool.getElement();
			_first->setData( in_data );
			_current = _first;
			_last = _first;
		}
		else{
			_last->next = _pool.getElement();
			_last->next = in_data;
			_last = _last->next;
			// not too sure about the following
			//current = last;
		}
		_size++;
		return;
	}

};

template <class T>
class ListPoolDynamic : public List<T>
{
protected:
	using List<T>::_first;
	using List<T>::_last;
	using List<T>::_current;
	using List<T>::_size;

	DynamicPool<T> _pool;

public:
	// constuctors
	ListPoolDynamic( int in_size=100 ) : List<T>(), _pool(in_size) {  }

	// i/o methods

	void append( const T &in_data ){
		if( _first == NULL ){
			_first = _pool.getElement();
			_first->data = in_data;
			_current = _first;
			_last = _first;
		}
		else{
			_last->next = _pool.getElement();
			_last->next = in_data;
			_last = _last->next;
			// not too sure about the following
			//current = last;
		}
		_size++;
		return;
	}

};

#endif
