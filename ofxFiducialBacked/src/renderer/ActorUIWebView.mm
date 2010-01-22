#ifdef TARGET_OF_IPHONE

#import "ActorUIWebView.h"
#import "iPhoneGlobals.h"

//--------------------------------------------------------------
// Objective C++ interface
@interface WebViewController : UIViewController<UIWebViewDelegate>
{
	IBOutlet UIWebView *webView;
}
@property (nonatomic, retain) UIWebView *webView;

- (void)setUrl:(NSString *)newUrl;
- (void)setOrigin:(ofPoint)newOrigin;
- (void)setAngle:(float)newAngle;

@end

@implementation WebViewController

@synthesize webView;

- (void)setUrl:(NSString *)newUrl
{	
	//Create a URL object.
	NSURL *url = [NSURL URLWithString:newUrl];
	
	//URL Requst Object
	NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
	
	//Load the request in the UIWebView.
	[webView loadRequest:requestObj];
}

- (void)setOrigin:(ofPoint)newOrigin
{
	CGPoint newPoint = {newOrigin.x, newOrigin.y};
	[webView setCenter:newPoint];
}

- (void)setAngle:(float)newAngle
{
	webView.transform = CGAffineTransformMakeRotation(newAngle);
}

@end

//--------------------------------------------------------------
// Stupid static accessors
class ActorUIWebView
{
public:
	ActorUIWebView()
	{
		setup();
		cout << "Make" << endl;
	}

	void setup()
	{
		UIWebViewWrapper = [[WebViewController alloc] initWithNibName:@"WebView" bundle:[NSBundle mainBundle]];
		[UIWebViewWrapper retain];
		[iPhoneGlobals.window addSubview:UIWebViewWrapper.webView];
		[iPhoneGlobals.window bringSubviewToFront:UIWebViewWrapper.webView];
		UIWebViewWrapper.webView.clearsContextBeforeDrawing = NO;
		UIWebViewWrapper.webView.opaque = NO;
	}
	~ActorUIWebView()
	{
		[UIWebViewWrapper.webView removeFromSuperview];
		[UIWebViewWrapper release];
		cout << "Break" << endl;
	}	
	WebViewController* UIWebViewWrapper;
};
static vector<ActorUIWebView> webViews;
//static ActorUIWebView webView;

//--------------------------------------------------------------
// C++ interface

namespace ofxFiducialBacked {
	namespace renderer {
		namespace iPhone {
			namespace render {
				void browser(ofRectangle frame, ofPoint origin, double angle, char* newUrl)
				{					
					static int slot;
					int browsersAvail = webViews.size();
					if (slot > browsersAvail-1)
						if (browsersAvail<MAX_UIWEBVIEW_BROWSERS)
							webViews.resize(browsersAvail+1);
						else
							slot = 0;
					
					WebViewController* viewWrapper = webViews[slot].UIWebViewWrapper;
/*					
					static bool bSetup;
					if (!bSetup)
					{
						webViews[slot].setup();
						bSetup = true;
					}
//					WebViewController* viewWrapper = webView.UIWebViewWrapper;
*/
					CGRect drawFrame = CGRectMake(frame.x, frame.y, frame.width, frame.height);
					[viewWrapper.webView setFrame:drawFrame];

					if (angle != 0.0)
						[viewWrapper setAngle:angle];
					if (newUrl != NULL)
					{
						NSString* url = [[NSString alloc]
										 initWithCString:newUrl
										 encoding:[NSString defaultCStringEncoding]];
						[viewWrapper setUrl:url];
					}
					
					[viewWrapper.webView setNeedsDisplay];
					slot++;
				}
			}
		}
	}
}

#endif