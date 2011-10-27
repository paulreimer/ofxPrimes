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


/*  Changes

	Code optimization by Jorge M Santiago

*/

#ifndef EC_FIDUCIALDATA
#define EC_FIDUCIALDATA

#include "point.h"
#include "list.h"

class SequenceStore{
protected:
	SequenceStore(){}
public:
	virtual void addSequence( int * seq, bool white ) = 0;
	virtual bool contains(const int * seq, const bool white) = 0;
	virtual ~SequenceStore(){}
};


class FiducialData
{
public:
	const static int _maxBranches = 256;

protected:
	int _type;
	int _sequence[_maxBranches+1];
	char _sequenceString[ (_maxBranches+1)*5 ];
	int _typeArray[_maxBranches+1];

	DTPoint _centre;
	int _width;
	int _height;

	int _angle;

	int _rootRegion;

	bool _whiteRoot;

	DTPoint _branchCentre[_maxBranches];
	int _branchIndex[_maxBranches];

	DTPoint _corner[4];

	int _size;

	//Tracking members
	int _ID;
	int _tracked;


public:
	static int getMax() { return _maxBranches; }
	static int getMaxLenght() { return _maxBranches; }

	FiducialData(void);
	~FiducialData(void);
	void init( int *in_sequence );

	int getType() const { return _type; }
	void setType(const int& in_type){ _type = in_type; }
	int& Type(){ return _type; }

	DTPoint getCentre() const { return _centre; }
	void setCentre(const DTPoint& in_centre){ _centre = in_centre; }
	DTPoint& Centre(){ return _centre; }

	int getSize() const { return _size; }
	void setSize(const int& in_size){ _size = in_size; }
	int& Size(){ return _size; }

	int getWidth() const { return _width; }
	void setWidth(const int& in_width){ _width = in_width; }
	int& Width(){ return _width; }

	int getHeight()  const { return _height; }
	void setHeight(const int& in_height){ _height = in_height; }
	int& Height(){ return _height; }

	int getAngle() const { return _angle; }
	void setAngle(const int& in_angle){ _angle = in_angle; }
	int& Angle(){ return _angle; }

	int getRootRegion() const { return _rootRegion; }
	void setRootRegion(const int& in_rootRegion){ _rootRegion = in_rootRegion; }
	int& RootRegion(){ return _rootRegion; }

	bool getWhiteRoot() const { return _whiteRoot; }
	void setWhiteRoot(const bool& in_whiteRoot){ _whiteRoot = in_whiteRoot; }
	bool& WhiteRoot(){ return _whiteRoot; }

	bool getColor() const { return _whiteRoot; }
	void setColor(const bool& in_whiteRoot){ _whiteRoot = in_whiteRoot; }
	bool& Color(){ return _whiteRoot; }

	DTPoint getBranchCentre(const int& i) const { return _branchCentre[i]; }
	void setBranchCentre(const int& i, const DTPoint& p);//{ branchCentre[i] = p; }
	DTPoint& BranchCentre(const int& i){ return _branchCentre[i]; }

	int getBranchIndex(const int& i);//{ return branchIndex[i]; }
	void setBranchCentre(const int& i, const int& index);//{ branchIndex[i] = index; }

	int * Sequence() { return _sequence; }
	const int * getSequence() const { return _sequence; }

	int * TypeArray() { return _typeArray; }
	const int * getTypeArray() const { return _typeArray; }

	#ifndef __SYMBIAN32__
	char * getSequenceString( bool colorInfo=false );
	#endif//ndef __SYMBIAN32__

	DTPoint getCorner(const int& i);//{ return corner[i]; }
	void setCorner(const int& i, const DTPoint& p);//{ corner[i] = p; }
	//DTPoint& Corner(const int& i){ return corner[i]; }

	//int * parseSequence( const char * in_fiducialSequence );
	static int * parseSequenceL( const char * in_fiducialSequence, bool& white );

	//Tracking
	void setID(const int& ID){ _ID = ID;}
	int getID()const {return _ID;}

	void setTracked(const int& tracked){ _tracked = tracked;}
	int getTracked() const {return _tracked;}

};

bool operator==(const FiducialData& a, const FiducialData& b);

#endif
