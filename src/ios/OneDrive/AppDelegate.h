//
//  AppDelegate.h
//  OneDrive
//
//  Created by Gaurav Srivastava on 3/18/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MasterDetailViewManager;

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) MasterDetailViewManager *masterDetailManager;
//@property (strong, nonatomic) UIViewController *rootViewController;
@end
