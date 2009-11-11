#import <QuartzCore/CAAnimation.h>

@interface PLCameraFocusView : UIView
{
    NSArray *_images;
    CAKeyframeAnimation *_contentsAnimation;
    CAKeyframeAnimation *_boundsAnimation;
}

- (id)init:(BOOL)arg1;
- (void)dealloc;
- (id)_createContentsAnimation;
- (id)_createBoundsAnimation;
- (void)startAnimating;
- (void)stopAnimating;
- (void)_fadeOut;
- (void)_fadeOutDidFinish;
- (void)focusDidEnd;

@end

@interface PLPreviewView : UIView
{
    struct CGPoint _touchLocation;
    PLCameraFocusView *_focusView;
    PLCameraFocusView *_autoFocusView;
    UIView *_disabledView;
    unsigned int _canShowFocus:1;
    unsigned int _controlsAreVisible:1;
}
@end

@interface PLPreviewView (RawData)
- (void)drawRect:(CGRect)rect;
- (void)setNeedsDisplay;
- (void)setNeedsDisplayInRect:(CGRect)invalidRect;
- (void)setNeedsLayout;

@end
