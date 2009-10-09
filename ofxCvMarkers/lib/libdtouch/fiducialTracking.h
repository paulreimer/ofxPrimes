/***************************************************************************
     fiducialdata.h  -  container class for fiducial recognition results
                             -------------------
    begin                : Sat Nov 27 2004
    copyright            : (C) 2004 by Enrico Costanza
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
#ifndef EC_FIDUCIALTRACKING
#define EC_FIDUCIALTRACKING

#include "pool.h"
#include "listhash.h"
#include "types.h"


class FiducialTracking
{
public:

	void increaseTrackingThreshold(){_trackingThreshold++;}
	void decreaseTrackingThreshold(){if(_trackingThreshold > 1 )_trackingThreshold--;}

	void increaseDistanceThreshold(){_distanceThreshold++;}
	void decreaseDistanceThreshold(){if(_distanceThreshold > 1 )_distanceThreshold--;}
	
	static FiducialTracking * newL(const int &distance,const int &tracking);
	FiducialTracking(const int &distance,const int &tracking);
	~FiducialTracking(void);

	void initL();

public:
	void process(FiducialDataList * curr);
	int _distanceThreshold;
	int _trackingThreshold;
protected:

	FiducialDataList* _pre;
	FiducialDataList * _filtered;
	int _idCount;
	int generateID();
};

#endif