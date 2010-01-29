#pragma once

#include "ofMain.h"

#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>

#include <cvd/gl_helpers.h>
/*
template <class pixel_t>
class ofxCvdImage
: public ofBaseDraws
, public ofBaseHasPixels
, public CVD::Image<pixel_t>
{
public:
	void draw(float x, float y)
	{
		draw(x, y, getWidth(), getHeight());
	}
	
	void draw(float x, float y, float w, float h)
	{
		glPushMatrix();
		{
			glRasterPos2f(x, y);
			glPixelZoom(w/getWidth(), -h/getHeight());
			glDrawPixels(*this);
		}
		glPopMatrix();
	}
	
	float getWidth()
	{
		return size().x;
	}
	float getHeight()
	{
		return size().y;
	}
	
	unsigned char* getPixels()
	{
		return (unsigned char*)data();
	}
};

typedef ofxCvdImage<CVD::byte> ofxCvdGrayscaleImage;
typedef ofxCvdImage<CVD::Rgb<CVD::byte> > ofxCvdColorImage;
*/

class ofxCvdGrayscaleImage
: public ofBaseDraws
, public ofBaseHasPixels
, public CVD::Image<CVD::byte>
{
public:
	void draw(float x, float y)
	{
		draw(x, y, getWidth(), getHeight());
	}
	
	void draw(float x, float y, float w, float h)
	{
		glPushMatrix();
		{
			glRasterPos2f(x, y);
			glPixelZoom(w/getWidth(), -h/getHeight());
			glDrawPixels(*this);
		}
		glPopMatrix();
	}
	
	float getWidth()
	{
		return size().x;
	}
	float getHeight()
	{
		return size().y;
	}
	
	unsigned char* getPixels()
	{
		return (unsigned char*)data();
	}
};

class ofxCvdColorImage
: public ofBaseDraws
, public ofBaseHasPixels
, public CVD::Image<CVD::Rgb<CVD::byte> >
{
public:
	void draw(float x, float y)
	{
		draw(x, y, getWidth(), getHeight());
	}
	
	void draw(float x, float y, float w, float h)
	{
		glPushMatrix();
		{
			glRasterPos2f(x, y);
			glPixelZoom(w/getWidth(), -h/getHeight());
			glDrawPixels(*this);
		}
		glPopMatrix();
	}
	
	float getWidth()
	{
		return size().x;
	}
	float getHeight()
	{
		return size().y;
	}
	
	unsigned char* getPixels()
	{
		return (unsigned char*)data();
	}
};
