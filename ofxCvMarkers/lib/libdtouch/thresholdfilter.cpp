/***************************************************************************
                       thresholdfilter.cpp  -  description
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

#include "thresholdfilter.h"

#include <math.h>
#include <assert.h>

#include <string.h>

// the following is from http://www.codecodex.com/wiki/index.php?title=Calculate_an_integer_square_root

/*
 * Return the truncated integer square root of "y" using longs.
 * Return -1 on error.
 */
long lsqrt(long y)
{
	long    x_old, x_new;
	long    testy;
	int     nbits;
	int     i;

	if (y <= 0) {
		if (y != 0) {
			//fprintf(stderr, "Domain error in lsqrt().\n");
			return -1L;
		}
		return 0L;
	}
	/* select a good starting value using binary logarithms: */
	nbits = (sizeof(y) * 8) - 1;    /* subtract 1 for sign bit */
	for (i = 4, testy = 16L;; i += 2, testy <<= 2L) {
		if (i >= nbits || y <= testy) {
			x_old = (1L << (i / 2L));       /* x_old = sqrt(testy) */
			break;
		}
	}
	/* x_old >= sqrt(y) */
	/* use the Babylonian method to arrive at the integer square root: */
	for (;;) {
		x_new = (y / x_old + x_old) / 2L;
		if (x_old <= x_new){
			break;
		}
		x_old = x_new;
	}
	/* make sure that the answer is right: */
	if ((long long) x_old * x_old > y || ((long long) x_old + 1) * ((long long) x_old + 1) <= y) {
		//fprintf(stderr, "Error in lsqrt().\n");
		return -1L;
	}
	return x_old;
}
// end of the code from http://www.codecodex.com/wiki/index.php?title=Calculate_an_integer_square_root


unsigned int thresholdOstu( unsigned char * img, unsigned int limit ){
	// calculate image histogram
	unsigned int histogram[256] = {0};
	int s[256];

	unsigned char *ip = img;
	for(unsigned int i=0;i<limit;i++){
		(*(histogram + *ip++))++;
	}

	unsigned int t=0;
	int histogramArea=0;
	unsigned int * histPointer = histogram;
	for( int i=0; i< 256; i++ ) {
		t+=*histPointer * i;
		histogramArea+=*histPointer++;
	}
	if (histogramArea)
		t/=histogramArea;
	else t=0;

	if( t<1 ) t++;
	if( t>254 ) t--;
	s[t] = sigma(histogram, t);
	s[t-1] = sigma(histogram, t-1);
	s[t+1] = sigma(histogram, t+1);

	if( !((s[t] > s[t+1]) && (s[t] > s[t-1])) ) {
		int delta=0;
		if( (s[t] <= s[t+1]) ) delta = +1;
		if( (s[t] <= s[t-1]) ) delta = -1;
		if( (s[t] < s[t+1]) ) delta = +1;
		if( (s[t] < s[t-1]) ) delta = -1;
		while( (t>1 && t<253) && (s[t] <= s[t+delta]) ) {
			t += delta;
			s[t+delta] = sigma(histogram, t+delta);
		}
	}
	return t;
}


int sigma( unsigned int * histo, unsigned int t )
{
	unsigned int i;
	long xAvg = 0;
	long x1Avg = 0;
	long w = 0; //= 32640; // 255 * 256 / 2
	long w1 = 0; //= t * (t+1) / 2;

	unsigned int * histPointer = histo;
	for( i=0; i<t; i++ ) {
		x1Avg += i * *histPointer;
		w1 += *histPointer++;
	}
	xAvg = x1Avg;
	w = w1;
	histPointer = histo;
	for( i=t; i<256; i++ ) {
		xAvg += i * *histPointer;
		w += *histPointer++;
	}

	// TODO: change the following to int?
	double x1d = x1Avg;
	double xd = xAvg;
	double wd = w;
	double w1d = w1;

	double termA = (x1d/w1d - xd/wd);
	double termB = w1d/(wd-w1d);

	return (int) ( termA*termA*termB );
}

void OstuThresholdFilter::setThreshold( unsigned char *img, unsigned char * thresholdBuffer ){
	// calculate image histogram
	unsigned int ostuT = thresholdOstu(img,_width*_height);
	memset(thresholdBuffer,ostuT,sizeof(thresholdBuffer));
}

void FixedThresholdFilter::setThreshold( unsigned char *img, unsigned char * thresholdBuffer ){
	memset(thresholdBuffer,127,sizeof(thresholdBuffer));
}

void BernsenThresholdFilter::constructL()
{
	_blockArea = _blockSize * _blockSize;
	_subBlockSize = (_blockSize >> 1);

	_noHorizSubBlocks = _width/_subBlockSize;
	_noVertSubBlocks = _height/_subBlockSize;
	_totalNoSubBlocks = _noHorizSubBlocks * _noVertSubBlocks;
	// end init of variables related to any local thresholding

	// start init of Bernsen related variables
	// !!! JMS
	_minBuffer = new unsigned int [_totalNoSubBlocks];
	_maxBuffer = new unsigned int [_totalNoSubBlocks];
}

BernsenThresholdFilter::~BernsenThresholdFilter(){
	// !!! JMS
	if (_minBuffer != NULL)
	{
		delete [] _minBuffer;
		_minBuffer = NULL;
	}

	if (_maxBuffer != NULL)
	{
		delete [] _maxBuffer;
		_maxBuffer = NULL;
	}
}

void SauvolaThresholdFilter::constructL()
{
	_blockArea = _blockSize * _blockSize;
	_subBlockSize = (_blockSize >> 1);

	_noHorizSubBlocks = _width/_subBlockSize;
	_noVertSubBlocks = _height/_subBlockSize;
	_totalNoSubBlocks = _noHorizSubBlocks * _noVertSubBlocks;
	// end init of variables related to any local thresholding

	// start init of Sauvola related variables
	_accBuffer = new unsigned int [_totalNoSubBlocks];
	_meanBuffer = new unsigned int [_totalNoSubBlocks];
	_varBuffer = new unsigned int [_totalNoSubBlocks];
}

SauvolaThresholdFilter::~SauvolaThresholdFilter(){
	// start cleanup buffers related to sauvola threshold
	if (_accBuffer != NULL)
	{
		delete [] _accBuffer;
		_accBuffer = NULL;
	}

	if (_meanBuffer != NULL)
	{
		delete [] _meanBuffer;
		_meanBuffer = NULL;
	}

	if (_varBuffer != NULL)
	{
		delete [] _varBuffer;
		_varBuffer = NULL;
	}
	// end cleanup buffers related to sauvola threshold
}

void SauvolaThresholdFilter::setMeanVarBuffersForThreshold(unsigned char *img){
	unsigned int m;
	unsigned int n;

	unsigned int accBufferOffset = 0;

	unsigned int x0;
	unsigned int y0;

	//unsigned int nXs;
	//unsigned int mXs;

	//unsigned int yXwidth;

	unsigned char *ip = img;

	unsigned int * accBufferPtr = NULL;

	unsigned int * meanBufferPtr = NULL;
	unsigned int * accBufferPtr00 = NULL;
	unsigned int * accBufferPtr10 = NULL;
	unsigned int * accBufferPtr01 = NULL;
	unsigned int * accBufferPtr11 = NULL;

	unsigned int * meanBufferPtr00 = NULL;
	unsigned int * meanBufferPtr01 = NULL;
	unsigned int * meanBufferPtr10 = NULL;
	unsigned int * meanBufferPtr11 = NULL;

	unsigned int * varBufferPtr00 = NULL;
	unsigned int * varBufferPtr01 = NULL;
	unsigned int * varBufferPtr10 = NULL;
	unsigned int * varBufferPtr11 = NULL;

	// reset to zero accumulator, mean and variance
	//memset(_accBuffer,0,_totalNoSubBlocks*sizeof(unsigned int));
	//memset(_meanBuffer,0,_totalNoSubBlocks*sizeof(unsigned int));
	//memset(_varBuffer,0,_totalNoSubBlocks*sizeof(unsigned int));
	for(unsigned int i=0;i<_totalNoSubBlocks;i++){
		_accBuffer[i] = 0;
		_meanBuffer[i] = 0;
		_varBuffer[i] = 0;
	}

	accBufferPtr = _accBuffer;
	int yOffset=0;
	// for each row of subBlocks
	for(n=0; n<_noVertSubBlocks;n++, accBufferOffset+=_noHorizSubBlocks){

		// it should be possible to replace this multiplication
		// with an accumulation in the for loop
		// i.e. for(n=0; n<_noVertSubBlocks;n++, accBufferOffset+=_noHorizSubBlocks)
		//unsigned int accBufferOffset = n * _noHorizSubBlocks;

		int xOffset = 0;
		// for each column of subBlocks
		for(m=0; m<_noHorizSubBlocks;m++){

			// x0 and y0 are the coordinates withing the subBlock
			// (going from 0 to _subBlockSize)
			// reset the accumulation buffer
			//accBufferPtr = _accBuffer + accBufferOffset;
			ip = img + xOffset + yOffset;

			// for each line inside the subBlock..
			for(y0=0;y0<_subBlockSize;y0++){
				// accumulate the pixel value
				// x0 is the ascissa relative to the origin of the subBlock
				for(x0=0;x0<_subBlockSize;x0++){
					// acc[m,n] += img[x,y];
					// advance the image pointer
					*accBufferPtr+= *ip++;
				}// end for(x0
				ip+= _width - _subBlockSize;
			}// end for(y0
			// advance the subBlockBuffer index
			xOffset += _subBlockSize;
			accBufferPtr++;
		}// end for(m
		yOffset += _subBlockSize * _width;
	}// end for(n

	// now _accBuffer contains the accumulation
	/*
	{
		unsigned int * tmpPtr = _accBuffer;
		cout << "_accBuffer" << endl;
		for(n=0; n<_noVertSubBlocks;n++){
			cout << setw(2) << n <<  ">";
			for(m=0; m<_noHorizSubBlocks;m++){
				cout << setiosflags(ios::fixed) << setw(6) << setprecision(1);
				cout << *tmpPtr++;
				//cout << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
	//*/

	// calculate the mean of each block
	meanBufferPtr = _meanBuffer;//+_noHorizSubBlocks+1;
	accBufferPtr00 = _accBuffer;
	accBufferPtr10 = _accBuffer+1;
	accBufferPtr01 = _accBuffer+_noHorizSubBlocks;
	accBufferPtr11 = _accBuffer+_noHorizSubBlocks+1;
	// note that n and m are never read, so their use is just to
	// keep track of the amount of pixels
	// and it would be the same to have them run
	// from 0 to _noVertSubBlocks-1 or _noHorizSubBlocks-1

	unsigned int blockAreaDIV2 = _blockArea >> 1;

	// TODO: could probably skip a few calculations here, as
	// the blocks on the border of the image are not usable
	for(n=1; n<_noVertSubBlocks;n++){
		for(m=1; m<_noHorizSubBlocks;m++){
			//mean[m,n] = accBuffer[m,n] + accBuffer[m-1,n] + accBuffer[m,n-1] + accBuffer[m-1,n-1];
			*meanBufferPtr = *accBufferPtr00++ + *accBufferPtr01++ +
								*accBufferPtr10++ + *accBufferPtr11++;
			//mean[m,n]/=_blockArea;
			//*meanBufferPtr/=_blockArea;
			*meanBufferPtr = *meanBufferPtr/_blockArea + ((*meanBufferPtr%_blockArea>blockAreaDIV2)?(1):(0));
			meanBufferPtr++;
		}
		// here we need to skip one column
		accBufferPtr00++;
		accBufferPtr10++;
		accBufferPtr01++;
		accBufferPtr11++;
		meanBufferPtr++;
	}

	// now _meanBuffer contains the mean
	/*
	{
		unsigned int * tmpPtr = _meanBuffer;
		cout << "_meanBuffer" << endl;
		for(n=0; n<_noVertSubBlocks;n++){
			for(m=0; m<_noHorizSubBlocks;m++){
				cout << setiosflags(ios::fixed) << setw(6) << setprecision(1);
				cout << *tmpPtr++;
				cout << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
	//*/

	ip = img;

	meanBufferPtr00 = _meanBuffer;
	meanBufferPtr01 = _meanBuffer+1;
	meanBufferPtr10 = _meanBuffer+_noHorizSubBlocks;
	meanBufferPtr11 = _meanBuffer+_noHorizSubBlocks+1;

	varBufferPtr00 = _varBuffer;
	varBufferPtr01 = _varBuffer+1;
	varBufferPtr10 = _varBuffer+_noHorizSubBlocks;
	varBufferPtr11 = _varBuffer+_noHorizSubBlocks+1;

	accBufferOffset = 0;
	yOffset = 0;
	ip = img;
	// TODO: could probably skip a few calculations here, as
	// the blocks on the border of the image are not usable
	// for each row of subBlocks
	for(n=1; n<_noVertSubBlocks;n++, accBufferOffset+=_noHorizSubBlocks){
		// x0 and y0 are the coordinates withing the block
		// (going from 0 to _subBlockSize)

		int xOffset = 0;
		// for each column of subBlocks
		for(m=1; m<_noHorizSubBlocks;m++){

			ip = img + xOffset + yOffset;

			// for each line inside the subBlock
			for(unsigned int y0=0;y0<_subBlockSize;y0++){
				for(unsigned int x0=0;x0<_subBlockSize;x0++){
					// advance the image pointer
					// this could maybe approximated with the
					// absolute value?
					*varBufferPtr00+= (*ip - *meanBufferPtr00) * (*ip - *meanBufferPtr00);
					*varBufferPtr01+= (*ip - *meanBufferPtr01) * (*ip - *meanBufferPtr01);
					*varBufferPtr10+= (*ip - *meanBufferPtr10) * (*ip - *meanBufferPtr10);
					*varBufferPtr11+= (*ip - *meanBufferPtr11) * (*ip - *meanBufferPtr11);
					ip++;
				}
				ip+= _width - _subBlockSize;
			}

			varBufferPtr00++;
			varBufferPtr01++;
			varBufferPtr10++;
			varBufferPtr11++;

			meanBufferPtr00++;
			meanBufferPtr01++;
			meanBufferPtr10++;
			meanBufferPtr11++;

			xOffset += _subBlockSize;
		}// end for(m
		// skip the last column
		varBufferPtr00++;
		varBufferPtr01++;
		varBufferPtr10++;
		varBufferPtr11++;

		meanBufferPtr00++;
		meanBufferPtr01++;
		meanBufferPtr10++;
		meanBufferPtr11++;

		yOffset += _subBlockSize * _width;

	}// end for(n

	/*
	varBufferPtr00 = _varBuffer;
	for(n=0; n<_noVertSubBlocks;n++){
		cout << setw(2) << n <<  ">";
		for(m=0; m<_noHorizSubBlocks;m++){
			cout << setiosflags(ios::fixed) << setw(6) << setprecision(1);
			cout << sqrt( (double)(*varBufferPtr00)/(double)_blockArea );
			//cout << " ";
			//cout << *varBufferPtr00;
			varBufferPtr00++;
		}
		cout << endl;
	}
	cout << setw(0);

	cout << "end of stddev table" << endl;
	//*/

	// pseudocode from wikipedia
	// for calculating the variance
	/*
	n = 0
	sum1 = 0
	foreach x in data:
		n = n + 1
		sum1 = sum1 + x
	end for
	mean = sum1/n

	sum2 = 0
	foreach x in data:
		sum2 = sum2 + (x - mean)^2
	end for
	variance = sum2/(n - 1)
	*/
}

void SauvolaThresholdFilter::setThreshold( unsigned char *img, unsigned char * thresholdBuffer )
{
	setMeanVarBuffersForThreshold( img );

	unsigned int m;
	unsigned int n;

	//unsigned int mXn;
	unsigned int mXs;
	unsigned int nXs;

	// calculate the threshold value for each block
	// and apply it
	unsigned int * varBufferPtr;// = _varBuffer = NULL;
	unsigned int * meanBufferPtr;// = _meanBuffer = NULL;

	unsigned char thresholdHere;

	unsigned int y;
	unsigned int yXwidth;

	unsigned char *tp;

	unsigned char sDIV2 = _subBlockSize >> 1;

	nXs = 0;
	unsigned int bufferOffset = 0;
	for(n = 1; n < _noVertSubBlocks; n++ ){
		varBufferPtr = _varBuffer + bufferOffset;
		meanBufferPtr = _meanBuffer + bufferOffset;
		//cout << "bufferOffset: " << bufferOffset << endl;
		mXs = 0;
		for(m = 1; m < _noHorizSubBlocks; m++){
			// from sauvola paper:
			// t = mean + mean *  k * (stdDev/R - 1)
			//   = mean + mean * k *  stdDev/R - mean * k
			const int R = 128;
			unsigned int kLSH10 = _sauvolaParam;
			//double stdDev_d = sqrt( (double)(*varBufferPtr)/(double)_blockArea );
			//int stdDev = (lsqrt( (long)(*varBufferPtr) ) << 10 )/ _blockArea;
			//int stdDevLSH10 = stdDev << 8;
			/*
			double stdDev = sqrt( (double)(*varBufferPtr)/(double)_blockArea );
			double t = *meanBufferPtr + *meanBufferPtr *  0.2 * (stdDev/R - 1);
			thresholdHere = (unsigned char) t;
			cout << "stdDev: " << stdDev << ", factor: " << (0.2 * (stdDev/R - 1)) << ", f: " << (*meanBufferPtr * 0.2 * (stdDev/R - 1)) << endl;
			//*/
			/*
			long var =  (long)(*varBufferPtr)/ _blockArea ;
			int stdDevLSH10 = lsqrt( var ) * 1024;
			int kLSH10 =  1024 / 5;
			int factor =  kLSH10 * stdDevLSH10 / R - kLSH10 * 1024;
			thresholdHere = *meanBufferPtr + ((*meanBufferPtr * factor)/(1024*1024));
			//*/

			long var =  (long)(*varBufferPtr)/ _blockArea ;
			unsigned int stdDevLSH10 = lsqrt( var ) << 10;//* 1024;
			int factor =  kLSH10 * stdDevLSH10 / R - (kLSH10 << 10);// 1024;
			thresholdHere = *meanBufferPtr + (((int)(*meanBufferPtr) * factor)/1048576);
			//thresholdHere = *meanBufferPtr + (((int)(*meanBufferPtr) * factor)>>20);

			/*
			cout << setiosflags(ios::fixed) << setw(6) << setprecision(3);
			cout << "stdDev: " << (stdDevLSH10>>10);
			cout << ", factor: " << factor << ", f:" << ((((int)(*meanBufferPtr) * factor)/1048576)) << endl;
			//*/
			varBufferPtr++;
			meanBufferPtr++;
			//apply threshold to 1/4 of the pixel in this cluster (central ones)
			//for(y = m*_subBlockSize+_subBlockSize/2; y<(m+1)*_subBlockSize+_subBlockSize/2; y++ )
			yXwidth = (nXs + sDIV2) * _width;
			for(y = 0; y<_subBlockSize; y++ ){
				tp = thresholdBuffer + mXs + sDIV2 + yXwidth;
				memset(tp, thresholdHere, _subBlockSize*sizeof(unsigned char));
				yXwidth += _width;
			}
			mXs += _subBlockSize;
		}
		nXs += _subBlockSize;
		bufferOffset += _noHorizSubBlocks;
	}

	return;
}


void BernsenThresholdFilter::setMinMaxBuffersForThreshold(unsigned char * img){
	unsigned int *minBufferPtr = _minBuffer;
	unsigned int *maxBufferPtr = _maxBuffer;

	unsigned int m;
	unsigned int n;

	unsigned int x;
	unsigned int y;

	unsigned int max;
	unsigned int min;

	unsigned int nXs;
	unsigned int mXs;

	unsigned int yXwidth;

	unsigned char *ip;

	y=0;
	nXs = 0;
	unsigned int yOffset = 0;
	unsigned int blockRowOffset = _subBlockSize*_width;
	for(n = 0; n < _noVertSubBlocks; n++ ){
		mXs = 0;
		for(m = 0; m < _noHorizSubBlocks; m++ ){
			max = 0;
			min = 255;

			// for(y = mXs; y < ((m+1) * _subBlockSize); ++y )
			//yXwidth = (nXs * _width);
			yXwidth = yOffset;
			//for(y = mXs; y < (mXs + _subBlockSize); y++ ){
			for(y = 0; y < _subBlockSize; y++ ){
				//for(x = 0, ip = img + nXs + (y*_width); x < _subBlockSize; ++x, ++ip )
				ip = img + mXs + yXwidth;
				//ip = img + mXs + yOffset;
				for(x = 0; x < _subBlockSize; x++ ){
					if( *ip > max ){
						max = *ip;
//					}else if( *ip < min ){
					}
					if( *ip < min ){
						min = *ip;
					}
					ip++;
				}
				yXwidth += _width;
			}

			*minBufferPtr++ = min;
			*maxBufferPtr++ = max;
			mXs += _subBlockSize;
		}
		//nXs += _subBlockSize;
		yOffset += blockRowOffset;
	}
}


void BernsenThresholdFilter::setSubClustersMinMax( const unsigned int &uOstuThreshold, unsigned char * thresholdBuffer ){
	unsigned int m;
	unsigned int n;

	//unsigned int mXn;
	unsigned int mXs;
	unsigned int nXs;

	unsigned int *minBufferCurrent;
	unsigned int *minBufferRight;
	unsigned int *minBufferBelow;
	unsigned int *minBufferBelowRight;

	unsigned int *maxBufferCurrent;
	unsigned int *maxBufferRight;
	unsigned int *maxBufferBelow;
	unsigned int *maxBufferBelowRight;

	unsigned int min;
	unsigned int max;

	unsigned char thresholdHere;

	unsigned int y;
	unsigned int yXwidth;

	unsigned char *tp;

	unsigned int sDIV2 = _subBlockSize >> 1;

	nXs = 0;
	unsigned int bufferOffset = 0;
	for(n = 1; n < _noVertSubBlocks; n++ ){
		minBufferCurrent    = _minBuffer + bufferOffset;
		minBufferRight      = _minBuffer + bufferOffset + 1;
		minBufferBelow      = _minBuffer + bufferOffset + _noHorizSubBlocks;
		minBufferBelowRight = _minBuffer + bufferOffset + _noHorizSubBlocks + 1;

		maxBufferCurrent    = _maxBuffer + bufferOffset;
		maxBufferRight      = _maxBuffer + bufferOffset + 1;
		maxBufferBelow      = _maxBuffer + bufferOffset + _noHorizSubBlocks;
		maxBufferBelowRight = _maxBuffer + bufferOffset + _noHorizSubBlocks + 1;

		//mXn = 0;
		mXs = 0;
		for(m = 1; m < _noHorizSubBlocks; m++){
			/*
			if( m==12 && n==14 ){
				cout << "halt";
			}//*/
			//calculate cluster min
			min = *minBufferCurrent++;
			( min > *minBufferRight ) ? ( min = *minBufferRight++ ) : ( *minBufferRight++ );
			( min > *minBufferBelow ) ? ( min = *minBufferBelow++ ) : ( *minBufferBelow++ );
			( min > *minBufferBelowRight ) ? ( min = *minBufferBelowRight++ ) : ( *minBufferBelowRight++ );
			//cluster min ready

			//calculate cluster max
			max = *maxBufferCurrent++;
			( max < *maxBufferRight ) ? ( max = *maxBufferRight++ ) : ( *maxBufferRight++ );
			( max < *maxBufferBelow ) ? ( max = *maxBufferBelow++ ) : ( *maxBufferBelow++ );
			( max < *maxBufferBelowRight ) ? ( max = *maxBufferBelowRight++ ) : ( *maxBufferBelowRight++ );
			//cluster max ready

			if( (max - min) < _contrThres ){
				//thresholdHere = uOstuThreshold;
				if( ((min + max) >> 1) > uOstuThreshold ){
					thresholdHere = 0;
				}else{
					thresholdHere = 255;
				}
			}else{
				//thresholdHere = (min + max) / 2;
				thresholdHere = (min + max) >> 1;
			}

			//apply threshold to 1/4 of the pixel in this cluster (central ones)
			//for(y = m*_subBlockSize+_subBlockSize/2; y<(m+1)*_subBlockSize+_subBlockSize/2; y++ )
			yXwidth = (nXs + sDIV2) * _width;
			for(y = 0; y<_subBlockSize; y++ ){
				tp = thresholdBuffer + mXs + sDIV2 + yXwidth;
				memset(tp, thresholdHere, _subBlockSize*sizeof(unsigned char));
				yXwidth += _width;
			}
			//end of cluster
			//cout << setw(4) << (unsigned int) thresholdHere;
			//cout << setw(4) << (unsigned int) min;

			//mXn += _noVertSubBlocks;
			mXs += _subBlockSize;
		}
		//cout << endl;
		nXs += _subBlockSize;
		bufferOffset += _noHorizSubBlocks;
	}

}

void BernsenThresholdFilter::setThreshold( unsigned char *img, unsigned char * thresholdBuffer )
{
	unsigned int limit = _width*_height;

	//calculate a global threshold, to be used when the std deviation is below a certain threshold
	//int ostuT = thresholdOstu();
	unsigned int uOstuT = thresholdOstu(img,_width*_height);

	//calculate 2 buffers with min and max of quarters of each cluster
	//this is used to reduce the number of operations (comparison) needed to calculate the mean
	setMinMaxBuffersForThreshold(img);

	//calculate actual min and max of each cluster, then contrast and threshold
	//for each cluster this is done combining 4 sub-clusters (i.e. the min and max values
	//of 4 sub-clusters are compared)
	memset(thresholdBuffer, 255, limit*sizeof(unsigned char));
	setSubClustersMinMax( uOstuT, thresholdBuffer );

	// need to handle the borders of the image separately
	//*
	unsigned int sDIV2 = _subBlockSize >> 1;
	unsigned int thresholdHere = 0;
	unsigned int x, y, m, n;//, nXs, nXn, mXn;
	unsigned int min, max, yXwidth, offset;
	unsigned int yStart;
	//#define _DBG_VALUE uOstuT;
	//#define _DBG_VALUE thresholdHere;

	//*
	// top left corner
	// from (0,0) to ( _subBlockSize/2, _subBlockSize/2 )
	min = _minBuffer[0];
	max = _maxBuffer[0];

	if( (max - min) < _contrThres ){
		//thresholdHere = uOstuT;
		if( ((min + max) >> 1) > uOstuT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//thresholdHere = uOstuT;
	yXwidth = 0;
	for(y=0;y<sDIV2;y++){
		//memset(threshold + yXwidth, thresholdHere, sDIV2);
		memset(thresholdBuffer + yXwidth, thresholdHere, sDIV2*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of top left corner
	//*/

	//*
	// top row
	// from ( _subBlockSize/2, 0 ) to ( _width - _subBlockSize/2, _subBlockSize/2 )
	int mXs = 0;
	for(m=1;m<_noHorizSubBlocks-1;m++){
		//calculate cluster min
		min = _minBuffer[m];
		if( min > _minBuffer[m+1] ){
			min = _minBuffer[m+1];
		}
		//cluster min ready

		//calculate cluster max
		max = _maxBuffer[m];
		if( max < _maxBuffer[m+1] ){
			max = _maxBuffer[m+1];
		}
		//cluster max ready

		if( (max - min) < _contrThres ){
			//thresholdHere = uOstuT;
			if( ((min + max) >> 1) > uOstuT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//thresholdHere = uOstuT;

		//apply threshold to the pixels in this cluster
		//for(y = 0, yXwidth = 0; y < _subBlockSize/2; ++y, yXwidth += _width)
		x = mXs + sDIV2;
		yXwidth = 0;
		for(y=0;y<sDIV2;y++){
			memset(thresholdBuffer + x + yXwidth, thresholdHere, _subBlockSize*sizeof(unsigned char));
			yXwidth += _width;
		}

		mXs += _subBlockSize;
		//end of cluster
	}// end of top row
	//*/

	//*
	// top right corner
	// from ( _width - _subBlockSize/2, 0 ) to ( _width, _subBlockSize/2 )
	min = _minBuffer[_noVertSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks
	max = _maxBuffer[_noVertSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks

	if( (max - min) < _contrThres ){
		//thresholdHere = uOstuT;
		if( ((min + max) >> 1) > uOstuT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//thresholdHere = uOstuT;
	//apply threshold to the pixels in this block
	//x = (_noHorizSubBlocks-2) * _subBlockSize;
	x = (_noHorizSubBlocks-1)*_subBlockSize + sDIV2;
	yXwidth = 0;
	for(y=0;y<sDIV2;y++){
		memset(thresholdBuffer + x + yXwidth, thresholdHere, (_width - x)*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of top right corner
	//*/

	offset = _width * (_height - 1 - _subBlockSize/2);
	//*
	// bottom left corner
	// from (0,_height - _subBlockSize/2) to ( _subBlockSize/2, _height )
	min = _minBuffer[_noHorizSubBlocks*(_noVertSubBlocks-1)];
	max = _maxBuffer[_noHorizSubBlocks*(_noVertSubBlocks-1)];

	if( (max - min) < _contrThres ){
		//thresholdHere = uOstuT;
		if( ((min + max) >> 1) > uOstuT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//thresholdHere = uOstuT;
	x = 0;
	//if( _height % _subBlockSize > sDIV2 ){
	//	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	//}else{
	//	yStart = sDIV2 + (_noVertSubBlocks-2)*_subBlockSize;
	//}
	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	offset = yStart * _width;

	yXwidth = offset;
	for(y=yStart;y<_height;y++){
		//memset(threshold + yXwidth, thresholdHere, sDIV2);
		memset(thresholdBuffer + x + yXwidth, thresholdHere, sDIV2*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of bottom left corner
	//*/

	//*
	// bottom row
	// from ( _subBlockSize/2,_height - _subBlockSize/2) to ( _width - _subBlockSize/2, _height )
	//offset = _width * (_height - _subBlockSize/2);
	mXs = 0;
	for(m=1+_noHorizSubBlocks*(_noVertSubBlocks-1);m<_totalNoSubBlocks-1;m++){
		//calculate cluster min
		min = _minBuffer[m];
		if( min > _minBuffer[m+1] ){
			min = _minBuffer[m+1];
		}
		//cluster min ready

		//calculate cluster max
		max = _maxBuffer[m];
		if( max < _maxBuffer[m+1] ){
			max = _maxBuffer[m+1];
		}
		//cluster max ready

		if( (max - min) < _contrThres ){
			//thresholdHere = uOstuT;
			if( ((min + max) >> 1) > uOstuT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//thresholdHere = uOstuT;
		//apply threshold to the pixels in this cluster
		//for(y = 0, yXwidth = 0; y < _subBlockSize/2; ++y, yXwidth += _width)
		x = mXs + sDIV2;
		yXwidth = offset;
		for(y=yStart;y<_height;y++){
			memset(thresholdBuffer + x + yXwidth, thresholdHere, _subBlockSize*sizeof(unsigned char));
			yXwidth += _width;
		}

		mXs += _subBlockSize;
		//end of cluster
	} //end of bottom row
	//*/

	//*
	// bottom right corner
	// from ( _width - _subBlockSize/2, 0 ) to ( _width, _subBlockSize/2 )
	min = _minBuffer[_totalNoSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks
	max = _maxBuffer[_totalNoSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks

	if( (max - min) < _contrThres ){
		//thresholdHere = uOstuT;
		if( ((min + max) >> 1) > uOstuT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//thresholdHere = uOstuT;
	//apply threshold to the pixels in this block
	// TODO: check the following
	//x = mXs + sDIV2;
	//if( _height % _subBlockSize > sDIV2 ){
	//	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	//}else{
	//	yStart = sDIV2 + (_noVertSubBlocks-2)*_subBlockSize;
	//}
	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	x = (_noHorizSubBlocks-1)*_subBlockSize + sDIV2;
	yXwidth = offset;
	for(y=yStart;y<_height;y++){
		memset(thresholdBuffer + x + yXwidth, thresholdHere, (_width - x)*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of bottom right corner
	//*/

	//*
	// left column
	// from (0,_height - _subBlockSize/2) to ( _subBlockSize/2, _height )
	//unsigned int yOffset = _subBlockSize * _width;
	yXwidth = sDIV2 * _width;
	x = 0;
	for(n=0;n<_noVertSubBlocks-1;n++){
		min = _minBuffer[_noHorizSubBlocks*n];
		max = _maxBuffer[_noHorizSubBlocks*n];

		if( (max - min) < _contrThres ){
			//thresholdHere = uOstuT;
			if( ((min + max) >> 1) > uOstuT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//thresholdHere = uOstuT;
		//yXwidth = offset;
		for(y=0;y<_subBlockSize;y++){
			//memset(threshold + yXwidth, thresholdHere, sDIV2);
			memset(thresholdBuffer + x + yXwidth, thresholdHere,  sDIV2*sizeof(unsigned char));
			yXwidth += _width;
		}
	}
	//end of left column
	//*/

	///*
	// right column
	// from (0,_height - _subBlockSize/2) to ( _subBlockSize/2, _height )
	//unsigned int yOffset = _subBlockSize * _width;
	//yXwidth = _subBlockSize * _width;
	yXwidth = sDIV2 * _width;
	x = (_noHorizSubBlocks-1)*_subBlockSize + sDIV2;
	for(n=0;n<_noVertSubBlocks-1;n++){
		min = _minBuffer[_noHorizSubBlocks*n];
		max = _maxBuffer[_noHorizSubBlocks*n];

		if( (max - min) < _contrThres ){
			//thresholdHere = uOstuT;
			if( ((min + max) >> 1) > uOstuT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//thresholdHere = uOstuT;
		//yXwidth = offset;
		for(y=0;y<_subBlockSize;y++){
			//memset(threshold + yXwidth, thresholdHere, sDIV2);
			memset(thresholdBuffer + x + yXwidth, thresholdHere,  (_width - x)*sizeof(unsigned char));
			yXwidth += _width;
		}
	}
	//end of right column
	//*/
}

void BernsenFixedThresholdFilter::setThreshold( unsigned char *img, unsigned char * thresholdBuffer )
{
	unsigned int limit = _width*_height;

	//calculate a global threshold, to be used when the std deviation is below a certain threshold
	//int ostuT = thresholdOstu();
	unsigned int fixedT = 127;

	//calculate 2 buffers with min and max of quarters of each cluster
	//this is used to reduce the number of operations (comparison) needed to calculate the mean
	setMinMaxBuffersForThreshold(img);

	//calculate actual min and max of each cluster, then contrast and threshold
	//for each cluster this is done combining 4 sub-clusters (i.e. the min and max values
	//of 4 sub-clusters are compared)
	memset(thresholdBuffer, 255, limit*sizeof(unsigned char));
	setSubClustersMinMax(fixedT, thresholdBuffer);

	// need to handle the borders of the image separately
	//*
	unsigned int sDIV2 = _subBlockSize >> 1;
	unsigned int thresholdHere = 0;
	unsigned int x=0, y=0, m=0, n=0;//, nXs, nXn, mXn;
	unsigned int min=0, max=0, yXwidth=0, offset=0;
	unsigned int yStart=0;

	unsigned int * minBufPtr = NULL;
	unsigned int * maxBufPtr = NULL;

	//*
	// top left corner
	// from (0,0) to ( _subBlockSize/2, _subBlockSize/2 )
	min = _minBuffer[0];
	max = _maxBuffer[0];

	if( (max - min) < _contrThres ){
		//thresholdHere = fixedT;
		if( ((min + max) >> 1) > fixedT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	yXwidth = 0;
	for(y=0;y<sDIV2;y++){
		//memset(threshold + yXwidth, thresholdHere, sDIV2);
		memset(thresholdBuffer + yXwidth, thresholdHere, sDIV2*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of top left corner
	//*/

	//*
	// top row
	// from ( _subBlockSize/2, 0 ) to ( _width - _subBlockSize/2, _subBlockSize/2 )
	int mXs = 0;
	minBufPtr = _minBuffer + 1;
	maxBufPtr = _maxBuffer + 1;
	for(m=1;m<_noHorizSubBlocks;m++){
		//calculate cluster min
		min = *minBufPtr;
		if( min > *(minBufPtr+1) ){
			min = *(minBufPtr+1);
		}
		//cluster min ready

		//calculate cluster max
		max = *maxBufPtr;
		if( max < *(maxBufPtr+1) ){
			max = *(maxBufPtr+1);
		}
		//cluster max ready

		minBufPtr++;
		maxBufPtr++;

		if( (max - min) < _contrThres ){
			//thresholdHere = fixedT;
			if( ((min + max) >> 1) > fixedT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//apply threshold to the pixels in this cluster
		//for(y = 0, yXwidth = 0; y < _subBlockSize/2; ++y, yXwidth += _width)
		x = mXs + sDIV2;
		yXwidth = 0;
		for(y=0;y<sDIV2;y++){
			memset(thresholdBuffer + x + yXwidth, thresholdHere, _subBlockSize*sizeof(unsigned char));
			yXwidth += _width;
		}

		mXs += _subBlockSize;
		//end of cluster
	}// end of top row
	//*/

	//*
	// top right corner
	// from ( _width - _subBlockSize/2, 0 ) to ( _width, _subBlockSize/2 )
	min = _minBuffer[_noVertSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks
	max = _maxBuffer[_noVertSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks

	if( (max - min) < _contrThres ){
		//thresholdHere = fixedT;
		if( ((min + max) >> 1) > fixedT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//apply threshold to the pixels in this block
	//x = (_noHorizSubBlocks-2) * _subBlockSize;
	x = (_noHorizSubBlocks-1)*_subBlockSize + sDIV2;
	yXwidth = 0;
	for(y=0;y<sDIV2;y++){
		memset(thresholdBuffer + x + yXwidth, thresholdHere, (_width - x)*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of top right corner
	//*/

	offset = _width * (_height - _subBlockSize/2 - 1);
	//*
	// bottom left corner
	// from (0,_height - _subBlockSize/2) to ( _subBlockSize/2, _height )
	min = _minBuffer[_totalNoSubBlocks - _noHorizSubBlocks];
	max = _maxBuffer[_totalNoSubBlocks - _noHorizSubBlocks];

	if( (max - min) < _contrThres ){
		//thresholdHere = fixedT;
		if( ((min + max) >> 1) > fixedT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	x = 0;
	// TODO: check the following!
	//if( _height % _subBlockSize > sDIV2 ){
	//	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	//}else{
	//	yStart = sDIV2 + (_noVertSubBlocks-2)*_subBlockSize;
	//}
	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	offset = yStart * _width;

	yXwidth = offset;
	for(y=yStart;y<_height;y++){
		//memset(threshold + yXwidth, thresholdHere, sDIV2);
		memset(thresholdBuffer + x + yXwidth, thresholdHere, sDIV2*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of bottom left corner
	//*/

	//*
	// bottom row
	// from ( _subBlockSize/2,_height - _subBlockSize/2) to ( _width - _subBlockSize/2, _height )
	//offset = _width * (_height - _subBlockSize/2);
	mXs = 0;
	minBufPtr = _minBuffer+1;
	maxBufPtr = _maxBuffer+1;
	for(m=1;m<_noHorizSubBlocks;m++){
		//calculate cluster min
		min = *minBufPtr;
		if( min > *(minBufPtr+1) ){
			min = *(minBufPtr+1);
		}
		//cluster min ready

		//calculate cluster max
		max = *maxBufPtr;
		if( max < *(maxBufPtr+1) ){
			max = *(maxBufPtr+1);
		}
		//cluster max ready
		minBufPtr++;
		maxBufPtr++;

		if( (max - min) < _contrThres ){
			//thresholdHere = fixedT;
			if( ((min + max) >> 1) > fixedT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//apply threshold to the pixels in this cluster
		//for(y = 0, yXwidth = 0; y < _subBlockSize/2; ++y, yXwidth += _width)
		x = mXs + sDIV2;
		yXwidth = offset;
		for(y=yStart;y<_height;y++){
			memset(thresholdBuffer + x + yXwidth, thresholdHere, _subBlockSize*sizeof(unsigned char));
			yXwidth += _width;
		}

		mXs += _subBlockSize;
		//end of cluster
	} //end of bottom row
	//*/

	//*
	// bottom right corner
	// from ( _width - _subBlockSize/2, 0 ) to ( _width, _subBlockSize/2 )
	min = _minBuffer[_totalNoSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks
	max = _maxBuffer[_totalNoSubBlocks-1]; //_noVertSubBlocks-1+0*_noVertSubBlocks

	if( (max - min) < _contrThres ){
		//thresholdHere = fixedT;
		if( ((min + max) >> 1) > fixedT ){
			thresholdHere = 0;
		}else{
			thresholdHere = 255;
		}
	}else{
		//thresholdHere = (min + max) / 2;
		thresholdHere = (min + max) >> 1;
	}

	//apply threshold to the pixels in this block
	// TODO: check the following!
	//if( _height % _subBlockSize > sDIV2 ){
	//	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	//}else{
	//	yStart = sDIV2 + (_noVertSubBlocks-2)*_subBlockSize;
	//}
	yStart = sDIV2 + (_noVertSubBlocks-1)*_subBlockSize;
	x = (_noHorizSubBlocks-1)*_subBlockSize + sDIV2;
	yXwidth = offset;
	for(y=yStart;y<_height;y++){
		memset(thresholdBuffer + x + yXwidth, thresholdHere, (_width - x)*sizeof(unsigned char));
		yXwidth += _width;
	}
	//end of bottom right corner
	//*/

	//*
	// left column
	// from (0,_height - _subBlockSize/2) to ( _subBlockSize/2, _height )
	//unsigned int yOffset = _subBlockSize * _width;
	yXwidth = sDIV2 * _width;
	x = 0;
	minBufPtr = _minBuffer + _noHorizSubBlocks*n;
	maxBufPtr = _maxBuffer + _noHorizSubBlocks*n;
	for(n=0;n<_noVertSubBlocks-1;n++){
		//calculate cluster min
		min = *minBufPtr;
		if( min > *(minBufPtr+_noHorizSubBlocks) ){
			min = *(minBufPtr+_noHorizSubBlocks);
		}
		//cluster min ready

		//calculate cluster max
		max = *maxBufPtr;
		if( max < *(maxBufPtr+_noHorizSubBlocks) ){
			max = *(maxBufPtr+_noHorizSubBlocks);
		}
		//cluster max ready
		minBufPtr+=_noHorizSubBlocks;
		maxBufPtr+=_noHorizSubBlocks;

		if( (max - min) < _contrThres ){
			//thresholdHere = fixedT;
			if( ((min + max) >> 1) > fixedT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//yXwidth = offset;
		for(y=0;y<_subBlockSize;y++){
			//memset(threshold + yXwidth, thresholdHere, sDIV2);
			memset(thresholdBuffer + x + yXwidth, thresholdHere,  sDIV2*sizeof(unsigned char));
			yXwidth += _width;
		}
	}
	//end of left column
	//*/

	///*
	// right column
	// from (0,_height - _subBlockSize/2) to ( _subBlockSize/2, _height )
	//unsigned int yOffset = _subBlockSize * _width;
	//yXwidth = _subBlockSize * _width;
	yXwidth = sDIV2 * _width;
	x = (_noHorizSubBlocks-1)*_subBlockSize + sDIV2;
	minBufPtr = _minBuffer + _noHorizSubBlocks-1;
	maxBufPtr = _maxBuffer + _noHorizSubBlocks-1;
	for(n=0;n<_noVertSubBlocks-1;n++){
		//calculate cluster min
		min = *minBufPtr;
		if( min > *(minBufPtr+_noHorizSubBlocks) ){
			min = *(minBufPtr+_noHorizSubBlocks);
		}
		//cluster min ready

		//calculate cluster max
		max = *maxBufPtr;
		if( max < *(maxBufPtr+_noHorizSubBlocks) ){
			max = *(maxBufPtr+_noHorizSubBlocks);
		}
		//cluster max ready
		minBufPtr+=_noHorizSubBlocks;
		maxBufPtr+=_noHorizSubBlocks;

		if( (max - min) < _contrThres ){
			//thresholdHere = fixedT;
			if( ((min + max) >> 1) > fixedT ){
				thresholdHere = 0;
			}else{
				thresholdHere = 255;
			}
		}else{
			//thresholdHere = (min + max) / 2;
			thresholdHere = (min + max) >> 1;
		}

		//yXwidth = offset;
		for(y=0;y<_subBlockSize;y++){
			//memset(threshold + yXwidth, thresholdHere, sDIV2);
			memset(thresholdBuffer + x + yXwidth, thresholdHere,  (_width - x)*sizeof(unsigned char));
			yXwidth += _width;
		}
	}
	//end of right column
	//*/
}



// end of file
