//
//  ListViewController.h
//  OneDrive
//
//  Created by Gaurav Srivastava on 5/29/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppController.h"

#pragma mark ListViewControllerDelegate protocol
// Delegate to listen to the PERTemplate selection made
@protocol ListViewControllerDelegate <NSObject>
@optional
- (void) callBackFromListViewController:(ODMedia)media forWorkflow:(NSInteger)workflow;
@end

@interface ListViewController : UITableViewController
{
    ODMediaList mFolderList;
    ODMedia mParentFolder;
    NSInteger mWorkflow;
}

@property (nonatomic, assign) id<ListViewControllerDelegate> delegate;

- (void) setParentMedia:(ODMedia)media forWorkflow:(NSInteger)workflow;

@end
