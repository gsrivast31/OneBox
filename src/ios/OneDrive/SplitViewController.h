//
//  SplitViewController.h
//  OneDrive
//
//  Created by Gaurav Srivastava on 4/7/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SplitViewController : UIViewController {
    UIView *_divider;
    NSInteger _width;
    NSInteger _detailStartPt;
}

@property (nonatomic, readonly) UIViewController *master;
@property (nonatomic, readonly) UIViewController *detail;
@property (strong, nonatomic) UIViewController *rootController;

- (void) setMasterController:(UIViewController *)controller;
- (void) setDetailController:(UIViewController *)controller;
- (void) setRootController:(UIViewController *)controller;
- (void) setMasterWidth:(NSInteger)w;

- (void) showRootController;
- (void) setSearchMode:(NSString*)query;
@end

