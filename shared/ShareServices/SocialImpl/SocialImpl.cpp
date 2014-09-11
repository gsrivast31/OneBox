//
//  SocialImpl.cpp
//  ShareServices
//
//  Created by Ruchin Kansal on 20/03/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#include "SocialImpl.h"
#include "StringConstants.h"
#import "Social/SLComposeViewController.h"
#import "Social/SLServiceTypes.h"

using namespace NewGroup::Core;

@interface ShareOnSocialPlatform:NSObject{
    ShareServiceInterface::ShareCallBackProc_t callback;
    NSString *initialText;
    NSNumber *serviceType;
}
-(void) setCallback:(ShareServiceInterface::ShareCallBackProc_t)cb;
-(void) setInitialText:(NSString *)text;
-(void) shareURL:(NSURL *)url;
-(void) setServiceType:(NSNumber *)service;
@end

@implementation ShareOnSocialPlatform
-(void) setInitialText:(NSString *)text{
    initialText = text;
}

-(void) setServiceType:(NSNumber *)service{
    serviceType = service;
}

-(void) setCallback:(ShareServiceInterface::ShareCallBackProc_t)cb{
    callback = cb;
}

-(void) shareURL:(NSURL *)url{
    UIViewController *topController = [UIApplication sharedApplication].keyWindow.rootViewController;
    
    while (topController.presentedViewController) {
        topController = topController.presentedViewController;
    }

    SLComposeViewController *composer;
    composer = [SLComposeViewController composeViewControllerForServiceType: [serviceType intValue] == kShareViaFacebook? SLServiceTypeFacebook:SLServiceTypeTwitter];
    
    [composer addURL:url];
    [composer setInitialText:initialText];
    
    [(SLComposeViewController *)composer setCompletionHandler:^(SLComposeViewControllerResult result)
     {
     [topController dismissViewControllerAnimated:YES completion:nil];
     callback(kNoError, UString(""));
     }];
    
    [topController presentViewController:composer animated:YES completion:nil];

}
@end

void SocialImpl::ShareURL(const UString &url, const UString& desc, ShareCallBackProc_t callback)
{
    ShareOnSocialPlatform *shareOnPlatform = [[ShareOnSocialPlatform alloc] init];
    [shareOnPlatform setInitialText:desc.getPlatformString()];
    [shareOnPlatform setCallback:callback];
    [shareOnPlatform setServiceType:[NSNumber numberWithInt:mServiceType]];
    
    [shareOnPlatform performSelectorOnMainThread:@selector(shareURL:) withObject:[NSURL URLWithString:url.getPlatformString()] waitUntilDone:NO];
}
