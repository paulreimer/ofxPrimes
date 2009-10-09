#include "testApp.h"
#include "ofxVectorMath.h"

bool	b[300];
float	f[300];
bool button = true;
int	iindex;

ofPoint *points;
ofPoint	v[300];

//--------------------------------------------------------------
void testApp::setup(){	 
	ofBackground(0, 0, 0);
	
	int bi = 0;
	int fi = 0;
	int vi = 0;
	
	gui.addSlider("twitchProbability", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("twitchAmount", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("twitchFlashAmount", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("tempBangAmount", f[fi++], 0.0, 20, 0.5); 
	gui.addSlider("tempTwitchAmount", f[fi++], 0.0, 5, 0.5); 
	
	gui.addSlider("noiseCurrentMax", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("noiseCurrentAmount", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("noiseCurrentScale", f[fi++], 0.0, 0.1, 0.5); 
	gui.addSlider("noiseCurrentSpeed", f[fi++], 0.0, 0.01,0.5); 
	
	gui.addSlider("maxSwimSpeed", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("swimFlockingSpeed", f[fi++], 0.0, 0.1, 0.5); 
	gui.addSlider("swimNoiseAmount", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("swimNoiseWiggleness", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("swimNoiseSpeed", f[fi++], 0.0, 0.01, 0.5); 
	
	gui.addSlider("saturation", f[fi++], 0.0, 1, 0.5); 
	
	gui.addSlider("uniqueness", f[fi++], 0.0, 5, 0.5); 
	
	gui.addSlider("ZOffset", f[fi++], 0.0, 1, 0.5); 
	
	gui.addSlider("dotAlpha", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("dotSizeMult", f[fi++], 0.0, 1, 0.5); 
	
	gui.addSlider("lineAlpha", f[fi++], 0.0, 1, 0.5); 
	gui.addSlider("lineSplineStep", f[fi++], 0.001, 0.1, 0.95); 
	gui.addSlider("lineSizeMult", f[fi++], 0.0, 1, 0.5); 
	
	gui.addSlider("ribbonFollowSpeed1", f[fi++], 0.0, 0.01, 0.5); 
	gui.addSlider("ribbon.maxOverall", f[fi++], 0, 20);
	
	gui.addSlider("audioVolume", f[fi++], 0, 1, 0); 
	gui.addSlider("audioVolumeChange", f[fi++], -.01, .01, 0); 
	gui.addSlider("audioTwitchThresh", f[fi++], 0, .005, 0); 
	
	gui.addSlider("dotSplineStep", f[fi++], 1.0/200, 1, 0.95); 
	
	gui.addSlider("cameraZ", f[fi++], 0, 500, 0.5); 
	gui.addSlider("Volume", f[fi++], 0, 1, 0.5);

	gui.addToggle("drawRibbonMode", b[bi++]);
	gui.addToggle("doDepth", b[bi++]);
	gui.addToggle("smoothing", b[bi++]);
	gui.addToggle("isOrthogonal", b[bi++]);
	
	
	gui.addToggle("mode.follow", b[bi++]);
	gui.addToggle("mode.drag", b[bi++]);	
	gui.addToggle("mode.orient", b[bi++]);	
	gui.addToggle("mode.repel", b[bi++]);	
	gui.addToggle("mode.gravity", b[bi++]);	
	gui.addToggle("options.useMouse", b[bi++]);	
	gui.addToggle("options.axisX", b[bi++]);	
	gui.addToggle("options.axisY", b[bi++]);	
	gui.addToggle("Bang", b[bi++]);	
	gui.addToggle("Twitch", b[bi++]);	
	
	
	
	gui.addSlider2d("Gravity", v[vi++], 0, 1, 3, 7);
	gui.addSlider2d("Wind", v[vi++], 0, 1, 3, 7);
	gui.addSlider2d("Current", v[vi++], 0, 1, 3, 7);
	gui.addSlider2d("Orientation", v[vi++], 0, 1, 3, 7);
	
		
	gui.addPage("My Second Page");
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	
	
	gui.addPage("My Final Page");
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);

	gui.addPage();		// gets name by default if name is omitted
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addSlider("Slider "+ ofToString(fi), f[fi++], 0, 1, 0.5);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	gui.addToggle("Toggle " + ofToString(bi), b[bi++]);
	
	//	
	//	gui.addOptionGroup("Group 1", iindex);
	//	gui.addOption("
	
	
	gui.loadFromXML();
}

//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
	gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	switch(key) {
		case ' ': gui.toggleDraw(); break;
		case '[': gui.nextPage(); break;
		case ']': gui.prevPage(); break;
		case '`': gui.nextPageWithBlank(); break;
	}
//	gui.nextPage();
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){

}
