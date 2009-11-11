#import "PLPreviewView+RawData.h"

@implementation PLPreviewView (RawData)

- (void)drawRect:(CGRect)rect
{
	NSLog(@"PLPreviewView told to drawRect");
	[ super drawRect:rect ];
}

- (void)setNeedsDisplay
{
	NSLog(@"PLPreviewView told setNeedsDisplay");
	[ super setNeedsDisplay ];
}

- (void)setNeedsDisplayInRect:(CGRect)invalidRect
{
	NSLog(@"PLPreviewView told setNeedsDisplayInRect");
	[ super setNeedsDisplayInRect:invalidRect ];
}

- (void)setNeedsLayout
{
	NSLog(@"PLPreviewView told setNeedsLayout");
	[ super setNeedsLayout ];
}

@end