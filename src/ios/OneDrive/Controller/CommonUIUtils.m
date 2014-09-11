//
//  CommonUIUtils.m
//  OneDriveUI
//
//  Created by Ruchin Kansal on 05/03/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import "CommonUIUtils.h"
#import "WrappedMediaData.h"
#import "AppController.h"


NSString* const kClipBoardForCut = @"com.onedrive.clipboard.cut";
NSString* const kClipBoardForCopy = @"com.onedrive.clipboard.copy";

@implementation CommonUIUtils

-(id) init{
    if(self = [super init]){

    }
    
    return self;
}

-(UIPasteboard *) getClipboard{
    return pasteBoard;
}

-(UIPasteboard *) createClipboard:(NSString *)operation{
    pasteBoard = [UIPasteboard pasteboardWithName:operation create:YES];
    return pasteBoard;
}

+(CommonUIUtils *) getInstance{
    static CommonUIUtils * pInstance = [[CommonUIUtils alloc] init];
    return pInstance;
}

-(NSNumber *) getActiveAccountID{
    return activeAccountId;
}

-(void) setActiveAccountID:(NSNumber *)accountId {
    activeAccountId = accountId;
}

-(void) showProgressNow:(NSString *)progressText{
    progressViewController.progressText = progressText;
    [rootViewController presentViewController:progressViewController animated:NO completion:nil];
}

-(void) showProgress:(NSString*)progressText{
    [self performSelectorOnMainThread:@selector(showProgressNow:) withObject:progressText waitUntilDone:YES];
}

-(void) initialize:(UIViewController*) rootView{
    rootViewController = rootView;
    progressViewController = [rootViewController.storyboard instantiateViewControllerWithIdentifier:@"progressViewController"];
}

-(void) hideProgressNow{
    [progressViewController dismissViewControllerAnimated:NO completion:nil];
}

-(void) hideProgress{
    [self performSelectorOnMainThread:@selector(hideProgressNow) withObject:nil waitUntilDone:YES ];
}

-(void) mediaUpdated:(ODMedia)media{
    WrappedMediaData *data = [[WrappedMediaData alloc] init];
    data.media = media;
    data.isDeleted = false;
    NSNotification *notify = [NSNotification notificationWithName:@"MediaUpdateNotification" object:data];
    [[NSNotificationCenter defaultCenter] postNotification:notify];
}

-(void) mediaDeleted:(ODMedia)media{
    WrappedMediaData *data = [[WrappedMediaData alloc] init];
    data.media = media;
    data.isDeleted = true;
    NSNotification *notify = [NSNotification notificationWithName:@"MediaUpdateNotification" object:data];
    [[NSNotificationCenter defaultCenter] postNotification:notify];
}

-(void) showErrorNow:(NSString*)operation{
    NSString* title = [operation stringByAppendingString:@" failed"];
    NSString* message = [operation stringByAppendingString:@"  failed for this file. Please try again later."];
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title
                                                    message:message
                                                   delegate:nil
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil];
    [alert show];
}

-  (void) showError:(NSString* ) operation{
    [self performSelectorOnMainThread:@selector(showErrorNow:) withObject:operation waitUntilDone:YES ];
}

-(NSString*) getServiceIcon:(int)serviceId {
    if (serviceId == NewGroup::Core::kSPDropbox)
        return @"Dropbox.png";
    else if(serviceId == NewGroup::Core::kSPGDrive)
        return @"Google-Drive-icon.png";
    else if (serviceId == NewGroup::Core::kSPSkyDrive)
        return @"SkyDrive.png";
    return @"";

}

@end


@implementation ImageUtils


+(UIImage*) drawText:(NSString*) text
             inImage:(UIImage*)  image
             atPoint:(CGPoint)   point
             offset: (CGPoint) offset 
{
    
    UIFont *font = [UIFont boldSystemFontOfSize:12];
    CGSize newSize = image.size;
    newSize.width += offset.x;
    newSize.height += offset.y;
    UIGraphicsBeginImageContext(newSize);
    [image drawInRect:CGRectMake(0,offset.y,image.size.width,image.size.height)];
    CGRect rect = CGRectMake(point.x, point.y, newSize.width, newSize.height);
    [[UIColor redColor] set];
    [text drawInRect:CGRectIntegral(rect) withFont:font];
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return newImage;
}

@end
