//
//  PopoverTableViewController.h
//  OneDrive
//
//  Created by Vikash Jindal on 19/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
#pragma mark PopoverTableViewControllerDelegate protocol
// Delegate to listen to the PERTemplate selection made
@protocol PopoverTableViewControllerDelegate <NSObject>

// Method to inform about search input selection
// index: the index of selected item
@optional
- (void) takenInputAtIndex:(UITableViewController *)popover selectedIndex:(NSInteger) index;

@end

@interface PopoverTableViewController : UITableViewController<UINavigationControllerDelegate>
{
    NSMutableArray*	_sectionsArray;
}

@property (nonatomic,assign) id<PopoverTableViewControllerDelegate> delegate;
@property (nonatomic, retain) NSMutableArray    *sectionsArray;
@property (nonatomic, assign) NSInteger selected;

- (void) initSectionArray:(NSInteger)section;
- (void) addToArray:(NSInteger)index objectToAdd:(id)object;

@end
