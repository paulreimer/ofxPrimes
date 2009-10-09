/***************************************************************************
          types.h  -  defines data types used in fiducial recognition
                      no implementation file
                             -------------------
    begin                : Wed Sep 18 2002
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

#ifndef EC_TYPES
#define EC_TYPES

#include "listpool.h"
#include "point.h"
#include  "fiducialdata.h"

#include <assert.h>

#include "intlist.h"

#define BOOL2COLOR(b) ((b==true)?("white"):("black"))

#ifndef __SYMBIAN32__
#define NEW new
#define CLN_PUSH(x)
#define CLN_POP(x)
//#define DT_LOG(X)
#else
//#define NEW new (ELeave)
#define CLN_PUSH(x) CleanupStack::PushL(x)
#define CLN_POP(x) CleanupStack::Pop(x)
//#define DT_LOG(x) RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, x )
#endif


//define new types based on the List<T> template
typedef List<class DTPoint> PointList;
typedef List<class FiducialData> FiducialDataList;
//typedef List<class dtouch_Region> RegionList;

typedef List<PointList *> PointListPtrList;

// NOTE: see intlist.h and intlist.cpp for intlist

//typedef std::deque<DTPoint> dtouch_Region;

//typedef Matrix<int> IntMatrix;
//typedef Matrix<bool> BoolMatrix;

#endif

