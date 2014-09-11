//
//  CommonUIUtils.h
//  OneDriveUI
//
//  Created by Ruchin Kansal on 05/03/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ProgressViewController.h"
#import "AppController.h"

extern NSString* const kClipBoardForCut;
extern NSString* const kClipBoardForCopy;

#define constMasterWidth 200.0f

@interface CommonUIUtils : NSObject
{
    UIPasteboard *pasteBoard;
    NSNumber *activeAccountId;
    UIViewController* rootViewController;
    ProgressViewController *progressViewController;
}
+(CommonUIUtils *) getInstance;
-(UIPasteboard *) getClipboard;
-(UIPasteboard *) createClipboard:(NSString *)operation;
-(NSNumber*) getActiveAccountID;
-(void) setActiveAccountID:(NSNumber*)accountId;
-(void) showProgress:(NSString*)progressText;
-(void) hideProgress;
-(void) initialize:(UIViewController*) rootView;
-(void) mediaUpdated:(ODMedia)media;
-(void) mediaDeleted:(ODMedia)media;
-(void) showError:(NSString*)operation;
-(NSString*) getServiceIcon:(int)serviceId;
@end



@interface ImageUtils : NSObject

+(UIImage*) drawText:(NSString*) text
             inImage:(UIImage*)  image
             atPoint:(CGPoint)   point
             offset: (CGPoint) offset;

@end