//
//  leftView.h
//  OneDrive_temp
//
//  Created by Ajay on 4/14/13.
//  Copyright (c) 2013 Ajay. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "leftTreeViewController.h"

@class SplitViewController;

@interface leftView : UIView
{
    
}

- (id)initWithFrame:(CGRect)frame;

@end

@interface leftViewController : UIViewController <leftTreeControllerDelegate>
{
    
}

- (id) initWithSplitViewController:(SplitViewController*)splitViewController;
- (void) callBackForLeftViewController : (NSIndexPath*)indexPath forObjectId:(NSNumber*)objectId;
- (void) setTableData : (NSArray *) data;

- (void) reloadLeftTree;

@property (nonatomic, strong) NSArray *sectionsData;
@property (nonatomic, strong) leftTreeViewController *_controller;
@property (nonatomic, retain) SplitViewController *splitViewController;

@end
