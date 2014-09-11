//
//  IconCell.m
//  OneDriveUI
//
//  Created by Vikash Jindal on 22/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//
#define MARGIN 5

#import "IconCell.h"
#import <QuartzCore/QuartzCore.h>
#import "ODCollectionViewLayoutAttributes.h"
static UIImage *deleteButtonImg; 

@implementation IconCell
@synthesize pController;


- (UString) getMediaPath
{
    return mediaPath;
}

-(void) showIndicator
{
    self.pController = [[UploadProgressViewController alloc] init];
    [self setAutoresizesSubviews:YES];
    self.pController.view.center = self.center;
    
    [self addSubview:self.pController.view];
  //uploadEventHandler.reset(FileUploadProgressManager::UploadFileProgressManager_t::SharedObserverType_t(updateProgress));
    uploadEventHandler = FileUploadProgressManager::instance().GetUploadNotificationManager()->MakeShared([self](FileUploadProgressManager::UploadInfoData_t uploadInfo){
        if(uploadInfo.get()->filePath == [self getMediaPath] && uploadInfo.get()->uploadStatus == YES)
              [self.pController stopProgress];

      }); 
    FileUploadProgressManager::instance().AddObserver(uploadEventHandler);

}

//- (id)initWithFrame:(CGRect)frame
//{
//    self = [super initWithFrame:frame];
//    if (self)
//    {
//        NSLog(@"Self height = %f, width = %f, x = %f, y = %f", self.bounds.size.height, self.bounds.size.width, self.bounds.origin.x, self.bounds.origin.y);
//        NSLog(@"Self: height = %f, width = %f, x = %f, y = %f", self.frame.size.height, self.frame.size.width, self.frame.origin.x, self.frame.origin.y);
//
//        UIView *insetView = [[UIView alloc] initWithFrame:CGRectInset(self.bounds, self.bounds.size.width/8, self.bounds.size.height/8)];
//        NSLog(@"insetView: height = %f, width = %f, x = %f, y = %f", insetView.bounds.size.height, insetView.bounds.size.width, insetView.bounds.origin.x, insetView.bounds.origin.y);
//        NSLog(@"insetView: height = %f, width = %f, x = %f, y = %f", insetView.frame.size.height, insetView.frame.size.width, insetView.frame.origin.x, insetView.frame.origin.y);
//
//        [self.contentView addSubview:insetView];
//        self.layer.shouldRasterize = YES;
//        self.label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, insetView.frame.size.width, insetView.frame.size.height)];
//        self.label.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleHeight;
//        self.label.textAlignment = NSTextAlignmentCenter;
//        self.label.numberOfLines = 3;
//        self.label.lineBreakMode = NSLineBreakByWordWrapping;
//        float dim = MIN(self.label.bounds.size.width, self.label.bounds.size.height);
//        self.label.clipsToBounds = YES;
//        self.label.layer.cornerRadius = dim/8;
//        self.label.layer.opacity = 0.7;
//        self.label.layer.borderColor = [UIColor darkGrayColor].CGColor;
//        self.label.layer.borderWidth = 1.0;
//        self.label.font = [UIFont systemFontOfSize:dim/6];
//        self.label.backgroundColor = [UIColor lightGrayColor];
//        [insetView addSubview:self.label];
//
//        self.deleteButton = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width/16, frame.size.width/16, frame.size.width/4, frame.size.width/4)];
//        if (!deleteButtonImg)
//        {
//            CGRect buttonFrame = self.deleteButton.frame;
//            UIGraphicsBeginImageContext(buttonFrame.size);
//            CGFloat sz = MIN(buttonFrame.size.width, buttonFrame.size.height);
//            UIBezierPath *path = [UIBezierPath bezierPathWithArcCenter:CGPointMake(buttonFrame.size.width/2, buttonFrame.size.height/2) radius:sz/2-MARGIN startAngle:0 endAngle:M_PI * 2 clockwise:YES];
//            [path moveToPoint:CGPointMake(MARGIN, MARGIN)];
//            [path addLineToPoint:CGPointMake(sz-MARGIN, sz-MARGIN)];
//            [path moveToPoint:CGPointMake(MARGIN, sz-MARGIN)];
//            [path addLineToPoint:CGPointMake(sz-MARGIN, MARGIN)];
//            [[UIColor blackColor] setFill];
//            [[UIColor whiteColor] setStroke];
//            [path setLineWidth:3.0];
//            [path fill];
//            [path stroke];
//            deleteButtonImg = UIGraphicsGetImageFromCurrentImageContext();
//            UIGraphicsEndImageContext();
//        }
//        [self.deleteButton setImage:deleteButtonImg forState:UIControlStateNormal];
//        [self.contentView addSubview:self.deleteButton];
//    }
//    return self;
//}

- (void)applyLayoutAttributes:(ODCollectionViewLayoutAttributes *)layoutAttributes
{
    if (layoutAttributes.isDeleteButtonHidden)
    {
        self.deleteButton.layer.opacity = 0.0;
        [self stopQuivering];
    }
    else
    {
        self.deleteButton.layer.opacity = 1.0;
        [self startQuivering];
    }
}

- (void)startQuivering
{
    CABasicAnimation *quiverAnim = [CABasicAnimation animationWithKeyPath:@"transform.rotation"];
    float startAngle = (-2) * M_PI/180.0;
    float stopAngle = -startAngle;
    quiverAnim.fromValue = [NSNumber numberWithFloat:startAngle];
    quiverAnim.toValue = [NSNumber numberWithFloat:3 * stopAngle];
    quiverAnim.autoreverses = YES;
    quiverAnim.duration = 0.2;
    quiverAnim.repeatCount = HUGE_VALF;
    float timeOffset = (float)(arc4random() % 100)/100 - 0.50;
    quiverAnim.timeOffset = timeOffset;
    CALayer *layer = self.layer;
    [layer addAnimation:quiverAnim forKey:@"quivering"];
}


- (void)stopQuivering
{
    CALayer *layer = self.layer;
    [layer removeAnimationForKey:@"quivering"];
}
@end

@implementation listViewCell

@end


@implementation UploadProgressView
@synthesize activityView;
@synthesize progressLabel;


@end


@implementation UploadProgressViewController
@synthesize mProgressView;
@synthesize mActivityView;

- (void) viewDidLoad
{
    /*
    mProgressView = [[[NSBundle mainBundle] loadNibNamed:@"uploadProgressView" owner:self options:nil] objectAtIndex:0];
    mProgressView.backgroundColor = [UIColor clearColor];
    [mProgressView setAutoresizesSubviews:YES];
    mProgressView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
    */
    mActivityView = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(0,0,50,50)];
    [mActivityView sizeToFit];
    
    [mActivityView setAutoresizesSubviews:YES];
    mActivityView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
    
    [self.view addSubview:mActivityView];
    self.view.frame = mActivityView.frame;
    [mActivityView startAnimating];
}

-(void) startProgress: (float)finished totalSize:(float)tSize
{

    [mActivityView startAnimating];
    
}
-(void) updateProgress: (float)finished totalSize:(float)tSize
{
    
}
-(void) userCancelled
{
    
}
-(void) stopProgress
{
    [mActivityView startAnimating];
 //   [mProgressView.activityView stopAnimating];
}

@end