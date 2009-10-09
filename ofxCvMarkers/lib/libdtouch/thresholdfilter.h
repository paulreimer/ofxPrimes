/***************************************************************************
                       thresholdfilter.h  -  description
                             -------------------
    begin                : Fri May 30 2008
    copyright            : (C) 2008 by Enrico Costanza
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

#ifndef EC_THRESHOLDFILTER
#define EC_THRESHOLDFILTER

#ifdef __SYMBIAN32__
#include <e32base.h>
#endif

unsigned int thresholdOstu( unsigned char * img, unsigned int limit );
int sigma( unsigned int *histo, unsigned int t );

class ThresholdFilter{
private:
	ThresholdFilter() { }

protected:
	unsigned int _width;
	unsigned int _height;

	ThresholdFilter( unsigned int in_width, unsigned int in_height ) :
		_width(in_width),
		_height(in_height)
	{ }

	void constructL();

public:
	virtual void setThreshold( unsigned char * img, unsigned char * thresholdBuffer ) = 0;

	virtual ~ThresholdFilter() { }

public:
	enum ThresholdType { EBernsen=0, EBernsenFixed, ESauvola, EOstu, EFixed } _thresholdMethod;

};

class OstuThresholdFilter : public ThresholdFilter
{
protected:
	void setThreshold( unsigned char *img, unsigned char * thresholdBuffer );
public:
	OstuThresholdFilter( const unsigned int &in_width, const unsigned int &in_height ):
		ThresholdFilter(in_width, in_height)
		{ }
	virtual ~OstuThresholdFilter() {};
};

class FixedThresholdFilter : public ThresholdFilter
{
protected:
	void setThreshold( unsigned char * img, unsigned char * thresholdBuffer );
public:
	FixedThresholdFilter( const unsigned int &in_width, const unsigned int &in_height ) :
		ThresholdFilter(in_width, in_height)
		{  }
	virtual ~FixedThresholdFilter() {};
};

class BernsenThresholdFilter : public ThresholdFilter
{
protected:
	unsigned int *_minBuffer;
	unsigned int *_maxBuffer;

	unsigned int _contrThres;

	unsigned int _blockSize;
	unsigned int _subBlockSize;
	unsigned int _blockArea;

	unsigned int _noHorizSubBlocks;
	unsigned int _noVertSubBlocks;
	unsigned int _totalNoSubBlocks;

	void setMinMaxBuffersForThreshold(unsigned char * img);
	void setSubClustersMinMax( const unsigned int &uOstuThreshold, unsigned char * thresholdBuffer );

	virtual void setThreshold( unsigned char *img, unsigned char * thresholdBuffer );

public:
	unsigned int getNoVertSubBlocks(){ return _noVertSubBlocks; }
	unsigned int getNoHorizSubBlocks(){ return _noHorizSubBlocks; }
	unsigned int getSubBlockSize(){ return _subBlockSize; }

	virtual ~BernsenThresholdFilter();

protected:
	BernsenThresholdFilter( const unsigned int &in_width, const unsigned int &in_height,
			const unsigned int &in_blockSize, const unsigned int &in_contrThres ) :
		ThresholdFilter(in_width,in_height),
		_contrThres(in_contrThres),
		_blockSize(in_blockSize)
	{ }
	
	void constructL();
public:
	static BernsenThresholdFilter * newL(const unsigned int &in_width, const unsigned int &in_height,
			const unsigned int &in_blockSize, const unsigned int &in_contrThres )
	{
		#ifndef __SYMBIAN32__
		BernsenThresholdFilter * self = new BernsenThresholdFilter(in_width,in_height,in_blockSize,in_contrThres);
		self->constructL();
		#else
		BernsenThresholdFilter * self = new BernsenThresholdFilter(in_width,in_height,in_blockSize,in_contrThres);
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}
};

class BernsenFixedThresholdFilter : public BernsenThresholdFilter
{
private:
	//FiducialRecognitionBernsenFixed(){}
public:
	virtual void setThreshold( unsigned char *img, unsigned char * thresholdBuffer );

	virtual ~BernsenFixedThresholdFilter() {}

	BernsenFixedThresholdFilter( const unsigned int &in_width, const unsigned int &in_height,
			const unsigned int &in_blockSize, const unsigned int &in_contrThres ) :
		BernsenThresholdFilter( in_width, in_height, in_blockSize, in_contrThres )
	{
		// do nothing
	}

	static BernsenFixedThresholdFilter * newL(const unsigned int &in_width, const unsigned int &in_height,
											  const unsigned int &in_blockSize, const unsigned int &in_contrThres )
	{
#ifndef __SYMBIAN32__
		BernsenFixedThresholdFilter * self = new BernsenFixedThresholdFilter(in_width,in_height,in_blockSize,in_contrThres);
		self->constructL();
#else
		BernsenFixedThresholdFilter * self = new BernsenFixedThresholdFilter(in_width,in_height,in_blockSize,in_contrThres);
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
#endif
		return self;
	}
};

class SauvolaThresholdFilter : public ThresholdFilter
{
protected:
	unsigned int *_accBuffer;
	unsigned int *_meanBuffer;
	unsigned int *_varBuffer;
	unsigned int _blockSize;
	unsigned int _subBlockSize;
	unsigned int _blockArea;

	unsigned int _noHorizSubBlocks;
	unsigned int _noVertSubBlocks;
	unsigned int _totalNoSubBlocks;

	unsigned int _sauvolaParam;

	void setMeanVarBuffersForThreshold(unsigned char *img);
	void setThreshold( unsigned char *img, unsigned char * thresholdBuffer );
public:
	unsigned int * getVarBuffer(){ return _varBuffer; }

	unsigned int getNoVertSubBlocks(){ return _noVertSubBlocks; }
	unsigned int getNoHorizSubBlocks(){ return _noHorizSubBlocks; }
	unsigned int getSubBlockSize(){ return _subBlockSize; }

	virtual ~SauvolaThresholdFilter();

protected:
	SauvolaThresholdFilter( const unsigned int &in_width, const unsigned int &in_height,
			const unsigned int &in_blockSize, const unsigned int &in_sauvolaParam ) :
		ThresholdFilter(in_width,in_height),
		_blockSize(in_blockSize),
		_sauvolaParam(in_sauvolaParam)
	{ }
	
	void constructL();
public:
	static SauvolaThresholdFilter * newL( const unsigned int &in_width, const unsigned int &in_height,
			const unsigned int &in_blockSize, const unsigned int &in_sauvolaParam )
	{
		#ifndef __SYMBIAN32__
		SauvolaThresholdFilter * self = new SauvolaThresholdFilter(
				in_width, in_height, in_blockSize, in_sauvolaParam );
		self->constructL();
		#else
		SauvolaThresholdFilter * self = new (ELeave) SauvolaThresholdFilter(
				in_width, in_height, in_blockSize, in_sauvolaParam );
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}
};


#endif // EC_THRESHOLDFILTER
// end of file


