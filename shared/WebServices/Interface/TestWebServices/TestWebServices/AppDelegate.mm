//
//  AppDelegate.m
//  TestWebServices
//
//  Created by Ruchin Kansal on 10/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#import "AppDelegate.h"

#import "ViewController.h"

#include "WebServicesInterface.h"
#include "CommonUtils.h"
#include "ODLogger.h"

@implementation AppDelegate

using namespace NewGroup::Core;

void ResponseHandler(int code, const WebServicesInterface::UserDataType_t& userData, const UString& response)
{
    Utils::Log(UString("Code: ") + UString(code) + UString(", Response: ") + response);
    std::vector<UString> keyValueComponents;
    response.Split(keyValueComponents, L"&");
    
    for(auto keyValue:keyValueComponents)
    {
        std::vector<UString> tokenComponents;
        keyValue.Split(tokenComponents, L"=");
        for(auto component:tokenComponents)
        {
            Utils::Log(component);
        }
    }
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    Utils::InitializeLogger("onedrive.log");
    Utils::Log("Hello World");
    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    // Override point for customization after application launch.
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPhone" bundle:nil];
    } else {
        self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPad" bundle:nil];
    }
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    
    /*auto callBackFunc = std::bind(&ResponseHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
    
    WebServicesInterface::ParametersType_t params;
    WebServicesInterface::UserDataType_t userData;
    params[UString(L"oauth_consumer_key")] = UString(L"337sy7vdvcycivz");
	params[UString(L"oauth_signature_method")] = UString(L"PLAINTEXT");
	params[UString(L"oauth_signature")] = UString(L"ez00yzm7xatak7s") + UString(L"&");
	params[UString(L"oauth_timestamp")] = Utils::GenerateTimestamp();
	params[UString(L"oauth_nonce")] = Utils::GenerateUUID();
    
    webServiceEngine->SendGetRequest(UString(L"https://api.dropbox.com/1/oauth/request_token"), params, userData);*/
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
