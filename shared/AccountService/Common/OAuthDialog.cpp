//
//  OAuthDialog.cpp
//  AccountServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//
#include "CommonUtils.h"
#include "OAuthDialog.h"
#include "ODLogger.h"

using namespace NewGroup::Core;

#ifdef BUILD_PLATFORM_IOS



@interface OAuthDialogView: UIWebView<UIWebViewDelegate> {
}
@property(nonatomic,assign) OAuthDialog* mAuthDialog; 
- (id)initWithObject:(OAuthDialog*)authDialog;
- (void)executeRequest;
@end

@implementation OAuthDialogView

- (id)initWithObject:(OAuthDialog*)authDialog
{
    if (!authDialog) return nil;
    
    auto t1 = authDialog->GetParentView();
    auto t2 = [t1 bounds];
    
    //auto t3 = [super alloc];
    
    @try{
    self = [super initWithFrame:t2];
    }
    @catch(NSException *excep)
    {
        NSLog(@"%@", excep);
    }
    
    //[super loadRequest:[NSURLRequest requestWithURL:URLToOpen]];
    
    //self = [super init];
    if (self) {
        CGRect rect = self.frame;
        self.mAuthDialog = authDialog;
        self.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
        t1.autoresizingMask = self.autoresizingMask;
        self.backgroundColor = nil;
        self.opaque = NO;
        super.delegate = self;
        self.scalesPageToFit = YES;
        
        
        
    }
    return self;
}

- (void)executeRequest{
    [self.mAuthDialog->GetParentView() addSubview:self];
    [self becomeFirstResponder];
    auto reqURL = [NSURL URLWithString:self.mAuthDialog->GetAuthURL().getPlatformString()];
    [super loadRequest:[NSURLRequest requestWithURL:reqURL]];
}

-(void) clearCookieForUrl:(NSString *)url
{
    NSHTTPCookieStorage* cookies = [NSHTTPCookieStorage sharedHTTPCookieStorage];
    NSArray* liveCookies = [cookies cookiesForURL:[NSURL URLWithString:url]];
    for (NSHTTPCookie* cookie in liveCookies)
    {
        [cookies deleteCookie:cookie];
    }
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType;
{
    /*if (![request.URL isEqual:URL]) {
		BOOL hasBeenHandled = NO;
		
		NSURL *redirectURL = authentication.configuration.redirectURL;
		if ([[request.URL absoluteString] hasPrefix:[redirectURL absoluteString]]) {
	        hasBeenHandled = [authentication handleRedirectURL:request.URL];
			if (hasBeenHandled) {
				[self close];
			}
			return NO;
		}
	}
    
	if (![[request.URL absoluteString] hasPrefix:[authentication.configuration.authURL absoluteString]]) {
		[[UIApplication sharedApplication] openURL:request.URL];
		return NO;
	}*/
	
    NSLog(@"%@", request);
    auto requestURL = [request.URL absoluteString];
    
    if ([requestURL hasPrefix:self.mAuthDialog->GetCallbackURL().getPlatformString()]) {
        std::map<UString, UString> paramKeyValues;
        Utils::ParseURL(UString::fromPlatform(requestURL), paramKeyValues);
        auto callBackProc = self.mAuthDialog->GetAuthCallbackProc();
        auto token = paramKeyValues.find(self.mAuthDialog->GetToken());
        if(token != paramKeyValues.end())
        {
            callBackProc(true, token->second);
        }
        else
        {
            callBackProc(false, UString());
        }
        return NO;
    }
    
	return YES;
}

@end

@interface OAuthDialogWrapper: NSObject
{
    
}
@property(nonatomic,assign) OAuthDialog* mAuthDialog;
- (void) performHandShake;
@end

@implementation OAuthDialogWrapper
- (void) performHandShake{
    auto authDialogView = [[OAuthDialogView alloc] initWithObject:self.mAuthDialog];
    self.mAuthDialog->SetDialogView(authDialogView);
    [authDialogView executeRequest];
}
@end

#endif
OAuthDialog::OAuthDialog(PlatformView parentView, const UString& url, const UString &callBackUrl, const UString &token, AuthCallBackProc_t callback):mParentView(parentView), mAuthURL(url), mCallbackURL(callBackUrl), mAuthCallbackProc(callback), mToken(token)
{
#ifdef BUILD_PLATFORM_IOS 
    auto wrapper = [[OAuthDialogWrapper alloc] init];
    wrapper.mAuthDialog = this;
    
    [wrapper performSelectorOnMainThread:@selector(performHandShake) withObject:nil waitUntilDone:YES];
    //mAuthDialogView = [[OAuthDialogView alloc] initWithObject:this];
    //mAuthDialogView = [[UIWebView alloc] initWithFrame:[parentView bounds]];
    
#endif
}

void OAuthDialog::ClearCookieForUrl(const UString& url)
{
#ifdef BUILD_PLATFORM_IOS 
    NSHTTPCookieStorage* cookies = [NSHTTPCookieStorage sharedHTTPCookieStorage];
    NSArray* liveCookies = [cookies cookiesForURL:[NSURL URLWithString:url.getPlatformString()]];
    for (NSHTTPCookie* cookie in liveCookies)
    {
        [cookies deleteCookie:cookie];
    }
#endif
}

OAuthDialog::~OAuthDialog()
{
    [mAuthDialogView removeFromSuperview];
}