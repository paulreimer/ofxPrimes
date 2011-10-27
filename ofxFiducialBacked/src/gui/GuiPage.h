#pragma once

#include "ofxSimpleGuiPage.h"

#include "ofxFiducialTracker.h"
#include "ofxFiducial.h"

#define USE_FIDUCIALS
#define NULL_FIDUCIAL_ID -1
#define FIDUCIAL_ORIGIN_CORNER 1

namespace ofxFiducialBacked {
	namespace gui {
		class GuiPage : public ofxSimpleGuiPage
		{
		public:
			GuiPage(string name="Fiducial Gui");
		//	~GuiPage();

			virtual void setup();
			virtual void update();
			virtual void draw();

			ofPoint	getOffset() { return offset;}
			float	getAngle()	{ return angle;	}

			void setOffset(ofPoint offset=ofPoint(0,0));
			void setAngle(float angle=0.0);
			void setScale(float scale=1.0);

			virtual void activate(bool isActivated = true);

			// these behave very similar to those in flash
			virtual void onRollOver(int x, int y)					{}
			virtual void onRollOut()								{}
			virtual void onMouseMove(int x, int y)					{}
			virtual void onDragOver(int x, int y, int button)		{}
			virtual void onDragOutside(int x, int y, int button)	{}
			virtual void onPress(int x, int y, int button)			{}
			virtual void onRelease(int x, int y, int button)		{}
			virtual void onReleaseOutside(int x, int y, int button)	{}

			virtual void keyPressed( int key ){}
			virtual void keyReleased( int key ){}

		protected:
			ofxSimpleGuiControl &addControl(ofxSimpleGuiControl& control);
			bool hitTest(int tx, int ty);
			void relocateMouseEvent(ofMouseEventArgs &e);

			void _mouseMoved(ofMouseEventArgs &e);
			void _mousePressed(ofMouseEventArgs &e);
			void _mouseDragged(ofMouseEventArgs &e);
			void _mouseReleased(ofMouseEventArgs &e);

			ofPoint offset;
			float scale;
			float angle;

			int saveX, saveY;
		};
	}
}
