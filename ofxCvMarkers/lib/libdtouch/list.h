/***************************************************************************
          list.h  -  simple linked list defined with a template
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Enrico Costanza
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

#ifndef EC_LIST
#define EC_LIST
#define EC_LIST_GRAPH

//#include <iostream>
//#include <fstream>
#ifndef __SYMBIAN32__
#include <cstring>
#endif

#ifdef __SYMBIAN32__
#include <string.h>
#include <e32base.h>
#endif

//using namespace std;

// TODO: if this is harmful, just rename List::check()
#ifdef check
#undef check
#endif

//#include <assert.h>

// warning: the template definition seems to be different on visual c++ from Linux
// (not 100% sure)
template<class T> class List;
//template<class T> class ListPool;
//template<class T> class StaticPool;

template <class T> class ListItem{
protected:
   ListItem *_next;
   T _data;

   //experimental: use at your own risk!
   //NOTE: used in listhash
   ListItem * _prev;

public:
	T getData() const {
		return _data;
	}

	void setData(T const &d){
		_data = d;
	}

	T& Data() {
		return _data;
	}

	ListItem<T> *getNext() const {
		return _next;
	}

	void setNext( ListItem<T> *n ){
		_next = n;
	}

	ListItem<T> *getPrev(){
		return _prev;
	}

	void setPrev( ListItem<T> *p ){
		_prev = p;
	}

   //constructors
   ListItem() : _next(NULL) { }
   ListItem( T in_data) : _next(NULL), _data(in_data) { }
   ListItem( T in_data, ListItem *in_next ) : _next(in_next), _data(in_data) { }

   ~ListItem() {
	   // delete _next; _next=NULL;
   }

	//bool deleted;

	/*
	void deleteCheck() {
		if( NULL!=_next && false==_next->deleted ){
			_next->deleteCheck();
		}
		deleted = true;
	}
	//*/

   friend class List<T>;
   //friend class ListPool<T>;
   //friend class StaticPool<T>;
};

template <class T> class List
{
protected:
	ListItem<T> * _first;
	ListItem<T> * _current;
	ListItem<T> * _last;

	int _size;

	// moveTo moves the current pointer to the first element in the List with value equal to target
	bool moveTo( const T target ){
		if( _first == NULL ){ return false; }
		_current = _first;
		while( _current != NULL )
		{
			if( _current->_data == target ){ return true; }
			_current = _current->_next;
		}
		return false;
	}

	// seek returns the pointer to the first element in the List with value equal to target
	ListItem<T> * seek( const T target ){
		if( _first == NULL ){ return NULL; }
		ListItem<T> *ptr = _first;
		while( ptr != NULL ){
			if( ptr->_data == target ){ return ptr; }
			ptr = ptr->_next;
		}
		return NULL;
	}

	List() { _current = _first = _last = NULL; _size=0; }

public:
	// constuctors
	static List * newL(){
		#ifndef __SYMBIAN32__
		return new List();
		#else
		return new (ELeave) List();
		#endif
	}

	// destructor
	~List() {
		// delete _first; _first = NULL;
		_current = _first;
		while( _current != NULL ){
			ListItem<T> *tmp = _current;
			_current = _current->_next;
			delete tmp;
		}
		_current = _first = _last = NULL;
		_size=0;
	}

	void empty() {
		// delete _first; _first=NULL;
		_current = _first;
		while( _current != NULL ){
			ListItem<T> *tmp = _current;
			_current = _current->_next;
			delete tmp;
		}
		_current = _first = _last = NULL;
		_size=0;
	}

	// i/o methods

	//void expand() { }
	const int &getSize(void) const { return _size; }
	//int size(void) { return _size; }

	ListItem<T> * getCurrent(void){
		return _current;
	}

	T getData(void){
		//assert( _current!=NULL );
		return( _current->_data );
		//if(_current!=NULL){ return( _current->_data ); }
		//else{ throw("\n\ngetData() called on a NULL pointer!!!\n\n"); }
	}

	T getFirstData(void){
		return( _first->_data );
		//if(_current!=NULL){ return( _current->_data ); }
		//else{ throw("\n\ngetData() called on a NULL pointer!!!\n\n"); }
	}

	void reset(void){ _current = _first; }

	void fwd(void){
		if(_current != NULL){ _current = _current->_next; }
	}

	T fwdGet(void){
		T result =_current->_data; _current = _current->_next; return result;
		//if(_current!=NULL){ _current = _current->_next; return _current->_data;  }
		//else{ throw("\n\nfwdGet() called on a NULL pointer!!!\n\n"); }
	}

	bool nullTest( void ) const {
			return( _current == NULL ); }
	bool fwdNullTest( void ) const {
			if( _current == NULL ) { return true; }
			return( _current->_next == NULL ); }

	bool isNull( void ) const {
			return( _current == NULL ); }
	bool isEmpty( void ){ return( _first == NULL ); }

	void appendL( const T &in_data ){
		// TODO: add handing _prev?
		if( _first == NULL ){
			#ifndef __SYMBIAN32__
			_first = new ListItem<T>(in_data);
			#else
			_first = new (ELeave) ListItem<T>(in_data);
			#endif
			_current = _first;

			_last = _first;
		}
		else{
			#ifndef __SYMBIAN32__
			_last->_next = new ListItem<T>( in_data, NULL );
			#else
			_last->_next = new (ELeave) ListItem<T>( in_data, NULL );
			#endif
			//if( _last->_next == NULL ){ throw( "\n\nList<T>::append: allocation failed!\n\n" ); }
			_last = _last->_next;
			// not sure about the following
			//_current = _last;
		}
		_size++;
		return;
	}

	void append( List<T> *src ){
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
			_last->_next = src->_first;
			_last = src->_last;
			// not too sure about the following
			//_current = _last;
		}
		_size += src->_size;

		src->_size = 0;
		src->_first = NULL;
		src->_current = NULL;
		src->_last = NULL;
		// TODO: add handing _prev?
		return;
	}

	void remove( const T &data ){
		if( _first == NULL ) { return; }

		if( _first->_data == data ){
			ListItem<T> *toBeDeleted = _first;
			_first = _first->_next;
			toBeDeleted->_next = NULL;
			if( _current == toBeDeleted ){ _current = _first; }
			if( _last == toBeDeleted ){
				_last = _first;
			}
			delete toBeDeleted;
			_size--;
			return;
		}

		ListItem<T> *prev = _first;
		ListItem<T> *toBeDeleted = _first->_next;

		while( toBeDeleted!=NULL ){
			if( toBeDeleted->_data == data ){
				prev->_next = toBeDeleted->_next;
				toBeDeleted->_next = NULL;
				if( _current == toBeDeleted ){ _current = prev; }
				if( _last == toBeDeleted ){
					_last = prev;
				}
				delete toBeDeleted;
				_size--;
				return;
			}

			prev = toBeDeleted;
			toBeDeleted = toBeDeleted->_next;
		}
		return;
	}

	void replace( const T &data_in, const T &data_out ){
		ListItem<T> *ptr = seek(data_out);
		// this is checked externally
		if( ptr!= NULL ){
			ptr->_data = data_in;
		}
	}

	bool check( const T &target ){
		if( _last!=NULL && _last->_data == target ){ return true; }
		ListItem<T> *ptr = _first;
		while( ptr != _last ){
			if( ptr->_data == target ){ return true; }
			ptr = ptr->_next;
		}
		return false;
	}

	bool contains ( const T target ){
		return (seek(target) != NULL);
	}

//	T * toArray(){
//		T * result = new T[_size+1];
//		*result = _size;
//		T * rPtr = result+1;
//		for(ListItem<T> ptr=_first; ptr!=NULL; ptr=ptr->_next){ *rPtr++ = ptr->_data }
//		//for(reset();!nullTest(); *rPtr++ = fwdGet()){}
//		//for(reset();!nullTest();fwd()){
//		//	*rPtr++ = getData();
//		//}
//		return result;
//	}
};

template <class T>
bool operator == ( const List<T> &a, const List<T> &b ){
	return false;
}



#endif


