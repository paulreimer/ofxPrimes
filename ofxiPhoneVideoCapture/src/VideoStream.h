#include "ofxiPhoneVideoCapture.h"

#undef	CREATE_SURFACE
#ifdef	CREATE_SURFACE
#import "Surface.h"
#import <CoreAnimation/CALayer.h>
#endif

@class PLCameraController;

@interface VideoStream : NSObject
{
	PLCameraController *cameraController;
	NSTimer *frameTimer;
	UIView *previewView;
	NSLock *pixelsUsage;
	
#ifdef FRAMEBUFFER_HACKS
//	CGBitmapContextRef ctx;
	unsigned char* bytes;
#endif

#ifdef CREATE_SURFACE
    CoreSurfaceBufferRef previewSurface;
    CALayer *previewLayer;
#endif

@public	
	ofxiPhoneVideoCapture *capture;
	ofImage image;
}

@property (nonatomic, retain) IBOutlet NSTimer *frameTimer;
@property ofxiPhoneVideoCapture *capture;
@property UIView *previewView;
#ifdef FRAMEBUFFER_HACKS
@property unsigned char *bytes;
#endif

@property ofImage image;

- (id)initWithCapture:(ofxiPhoneVideoCapture*)_capture;
- (void)frame;

@end
