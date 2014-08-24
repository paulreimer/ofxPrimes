#import <QuartzCore/CALayer.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <UIKit/UIKit.h>

#import "VideoStream.h"
#import "Surface.h"
#import "SurfaceAccelerator.h"
#import "Camera.h"

//#import "PLPreviewView+RawData.h"

#include "ofxiPhone.h"

OBJC_EXPORT unsigned int CGBitmapGetFastestAlignment();
OBJC_EXPORT void * CGBitmapAllocateData(unsigned int);
OBJC_EXPORT void CGBitmapFreeData(const void *data);

@class AVCapture;

@implementation VideoStream

@synthesize frameTimer;
@synthesize capture;
@synthesize previewView;
@synthesize image;
#ifdef FRAMEBUFFER_HACKS
@synthesize bytes;
#endif

- (id)initWithCapture:(ofxiPhoneVideoCapture*)_capture
{
	capture = _capture;
	[self init];

	return self;
}

- (id)init
{
	cameraController= [PLCameraController performSelector:@selector(sharedInstance)];
	previewView		= [cameraController performSelector:@selector(previewView)];

#ifdef FRAMEBUFFER_HACKS
	bytes = NULL;
#endif
	//	previewView.frame = CGRectMake(0,0, 320, 480);

	[previewView retain];
	[cameraController retain];

	[iPhoneGlobals.window addSubview:previewView];
	[iPhoneGlobals.window sendSubviewToBack:previewView];

	iPhoneGlobals.glView.opaque = NO;

	[cameraController performSelector:@selector(startPreview)];

	[cameraController setDelegate:iPhoneGlobals.appDelegate];
	[cameraController setFocusDisabled:NO];
	//	[cameraController setCaptureAtFullResolution:YES];
	[cameraController setDontShowFocus:YES];
	[cameraController setCameraMode:1];

#ifdef FRAMEBUFFER_HACKS
#ifdef CREATE_SURFACE
	CFMutableDictionaryRef dict;
	int x = 320, y = 480, pitch = x * 2, size = 2 * x * y, i;
	char *pixelFormat = "565L";

	dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
									 &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(dict, kCoreSurfaceBufferGlobal, kCFBooleanTrue);
	CFDictionarySetValue(dict, kCoreSurfaceBufferPitch,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pitch));
	CFDictionarySetValue(dict, kCoreSurfaceBufferWidth,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &x));
	CFDictionarySetValue(dict, kCoreSurfaceBufferHeight,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &y));
	CFDictionarySetValue(dict, kCoreSurfaceBufferPixelFormat,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
										pixelFormat));
	CFDictionarySetValue(dict, kCoreSurfaceBufferAllocSize,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &size));

	CGRect rect = [ UIHardware fullScreenApplicationContentRect ];
    rect.origin.x = rect.origin.y = 0.0f;

	previewSurface = CoreSurfaceBufferCreate(dict);
	previewLayer = [ [ CALayer layer ] retain ];
	[ previewLayer setFrame: rect ];
	[ previewLayer setContents: previewSurface ];
	[ previewLayer setOpaque: YES ];
#endif
#endif

	usleep(1000*100); // 100 ms

	if (capture->bThreaded)
		self.frameTimer = [NSTimer
						   scheduledTimerWithTimeInterval:1.0/60.0
						   target:self selector:@selector(frame)
						   userInfo:nil repeats:YES];

	return self;
}

- (void)frame
{
	if (!capture)
		return;

	CoreSurfaceBufferRef coreSurfaceBuffer;
#ifdef CREATE_SURFACE
	coreSurfaceBuffer = previewSurface;
#else
	coreSurfaceBuffer = [cameraController _createPreviewIOSurface];
#endif

//	CALayer* cameraLayer = previewView.layer;
//	cout << "CoreSurfaceBufferRef @" << coreSurfaceBuffer << endl;

	if (!coreSurfaceBuffer)
		return;

	Surface *surface = [[Surface alloc]initWithCoreSurfaceBuffer:coreSurfaceBuffer];
	[surface lock];

	int width = surface.width;
	int height = surface.height;

	if (!capture->width || !capture->height)
		capture->setSize(width, height);

	unsigned int bytesPerRow = surface.bytesPerRow;
	unsigned char* pixels = (unsigned char*)surface.baseAddress;

#ifdef FRAMEBUFFER_HACKS
	if (bytes==NULL)
		bytes = (unsigned char*)malloc(bytesPerRow*height);

	CGColorSpaceRef rgbcolor=CGColorSpaceCreateDeviceRGB();
	CGContextRef ctx = CGBitmapContextCreate(bytes, width, height, 8, width<<2, rgbcolor, kCGImageAlphaNoneSkipLast);

	memset(bytes, 0, bytesPerRow*height);
//	[subLayer renderInContext:ctx];
//	[previewView.layer renderInContext:ctx];

	CGContextRelease(ctx);
#endif

	int imageType;
	switch (bytesPerRow/width)
	{
		case 4:
			imageType = OF_IMAGE_COLOR_ALPHA;
			break;
		case 3:
			imageType = OF_IMAGE_COLOR;
			break;
		case 1:
			imageType = OF_IMAGE_GRAYSCALE;
			break;
		default:
			imageType = OF_IMAGE_UNDEFINED;
			break;
	}

	// TODO: Why is this inverted? offset error?
	capture->input.setFromPixels(pixels, width, height, imageType, false);

#ifdef FRAMEBUFFER_HACKS
	memset(pixels, 0xff, width*height*4/2);
#endif

	[surface unlock];
	[surface release];

	[previewView setNeedsDisplay];

	CFRelease(coreSurfaceBuffer);

	capture->update();
}

- (void)stop
{
	if (capture->bThreaded)
	{
		[frameTimer invalidate];
		[frameTimer release];

		[previewView removeFromSuperview];
	}

	[cameraController stopPreview];
}

- (void)dealloc
{
	[previewView release];
	[cameraController release];

#ifdef CREATE_SURFACE
	[ screenLayer release ];
#endif

    [super dealloc];
}
@end
