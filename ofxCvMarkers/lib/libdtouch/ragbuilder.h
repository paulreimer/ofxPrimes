/***************************************************************************
                          RAGBuilder.h  -  description
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
#ifndef EC_RAGBUILDER
#define EC_RAGBUILDER

#include "regionadjacencygraph.h"

class RAGBuilder{
	//_image(in_image), _width(in_width), _height(in_height),
	//	_threshold(in_threshold), _rag(in_rag), _stored(in_stored){

protected:
	//const int _width;
	//const int _height;

	//const unsigned char *_image;
	//const unsigned char *_threshold;

	//bool *_stored;
	
	//RegionAdjacencyGraph *_rag;

	//RAGBuilder() {}
public:
	
	//RAGBuilder( const unsigned char *in_image, const int &in_width, const int &in_height,
	//	const unsigned char *in_threshold, RegionAdjacencyGraph *in_rag );
	//~RAGBuilder();

	//int buildRAGL( const int &maxRegions=3000 );
	//int buildRAGBorderL( const int &maxRegions=3000 );
	static int buildRAGFullBorderL( const unsigned char *in_image, 
			const unsigned char *in_threshold, RegionAdjacencyGraph *in_rag );
};

#endif

