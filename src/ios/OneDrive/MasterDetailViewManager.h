//
//  DetailViewManager.h
//  OneDrive
//
//  Created by labuser on 3/20/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MasterDetailViewManager : NSObject <UISplitViewControllerDelegate, UITabBarControllerDelegate>

-(id)initWithSplitViewController:(UISplitViewController*)splitViewController
       withDetailRootControllers:(NSArray*)detailControllers;
-(void)setDetailController:(UIViewController *)viewController;

@end
