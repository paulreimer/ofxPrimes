#import "VideoStream.h"
#import "iPhoneGlobals.h"

// C++ bridge

static VideoStream* stream;

ofxiPhoneVideoCapture::ofxiPhoneVideoCapture()
{
	bThreaded = true;
}

ofxiPhoneVideoCapture::~ofxiPhoneVideoCapture()
{
	destroy();
}

void ofxiPhoneVideoCapture::setup()
{
	stream = [[VideoStream alloc] initWithCapture:this];
	[stream retain];
	
	[iPhoneGlobals.window addSubview:stream.previewView];
	[iPhoneGlobals.window sendSubviewToBack:stream.previewView];
	
	//	previewView.clearsContextBeforeDrawing = YES;
	//	previewView.opaque = YES;
	
	iPhoneGlobals.glView.opaque = NO;
}

void ofxiPhoneVideoCapture::capture()
{
	if (!bThreaded)
		[stream frame];
}

void ofxiPhoneVideoCapture::destroy()
{
	if (bThreaded)
	{
		[stream.frameTimer invalidate];
		[stream.frameTimer release];
		
		[stream.previewView removeFromSuperview];
	}
		
	[stream release];
}
