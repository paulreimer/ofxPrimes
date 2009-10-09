/***************************************************************************
	binarytree.h
	
	A template binary tree. The template implicitely rely on the existence
	of a function 
	int cmp( const T& a, const T& b )
	similar to strcmp.
	The tree is based on the C code published on
	http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/tree.html

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

#ifndef _EC_BINARYTREE
#define _EC_BINARYTREE

#include <iostream>

using namespace std;

/*
char * seq2String( int * x, char * buf){
	for(int i=0;i<=x[0];i++){
		//cout << "x["<<i<<"]: " << x[i] << endl;
		char tmp[] = "99,";
		sprintf(tmp,"%d,", x[i]);
		strcat(buf,tmp);
	}
	return buf;
}//*/

int cmp( const int &a, const int &b );
int cmp( const int * a, const int * b );

template <class T> class TreeNode{
public:
	T _value;
	TreeNode<T> * _right, * _left;
	
	TreeNode( const T& item ){ _value = item; _right = NULL; _left = NULL; }
	
	~TreeNode(){ 
		if(_right!=NULL){
			delete _right; _right=NULL;
		}
		if(_left!=NULL){
			delete _left; _left=NULL;
		}
	}
};

template <class T> class BinaryTree{
public:
	BinaryTree() { _root = NULL; }
	~BinaryTree() { if(_root != NULL){ delete _root; _root=NULL;} }
	
	TreeNode<T> * _root;

	void apply( void(*f)(T) ){
		if( _root != NULL ){
			f(_root->_value);
			apply( f, _root->_left );
			apply( f, _root->_right );
		}
		return;
	}
	
	void apply( void(*f)(T), TreeNode<T> * node ){
		if( node != NULL ){
			f(node->_value);
			apply( f, node->_left );
			apply( f, node->_right );
		}
		return;
	}
	
	void insert( const T& item ) {
		TreeNode<T> * newNode = new TreeNode<T>( item );
		if( _root == NULL ){
			_root = newNode;
			return;
		}

		if( cmp( item, _root->_value ) < 0 ){
			this->insert( newNode, &_root->_right );
		}else{
			this->insert( newNode, &_root->_left );
		}
	}
	
	void insert( TreeNode<T> * newNode, TreeNode<T> ** node ) {
		if( *node == NULL ){
			*node = newNode;
			return;
		}
		
		if( cmp( newNode->_value, (*node)->_value ) < 0 ){
			this->insert( newNode, &((*node)->_right) );
		}else{
			this->insert( newNode, &((*node)->_left) );
		}
	}

	bool contains( const T item ){
		if( _root == NULL ){ return false; }
		int comparisonResult = cmp(item,_root->_value);
		if( comparisonResult == 0 ){
			return true;
		}

		if( comparisonResult < 0 ){
			if( _root->_right == NULL ){ return false; }
			return this->contains( item, _root->_right );
		}else{
			if( _root->_left == NULL ){ return false; }
			return this->contains( item, _root->_left );
		}
	}

	bool contains( const T item, TreeNode<T> * node ){
		int comparisonResult = cmp(item,node->_value);
		if( comparisonResult == 0 ){
			return true;
		}

		if( comparisonResult < 0 ){
			if( node->_right == NULL ){ return false; }
			return this->contains( item, node->_right );
		}else{
			if( node->_left == NULL ){ return false; }
			return this->contains( item, node->_left );
		}
	}

};

#endif //_EC_BINARYTREE
