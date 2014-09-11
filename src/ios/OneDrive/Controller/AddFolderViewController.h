//
//  AddFolderViewController.h
//  OneDriveUI
//
//  Created by Gaurav Srivastava on 3/8/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
#pragma mark AddFolderViewControllerDelegate protocol
// Delegate to listen to the PERTemplate selection made
@protocol AddFolderViewControllerDelegate <NSObject>
@optional
- (void) removePopovers;
- (void) createFolder:(NSString*)folderName;
@end

@interface AddFolderViewController : UIViewController
@property (nonatomic, assign) id<AddFolderViewControllerDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *createButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *cancelButton;
@property (weak, nonatomic) IBOutlet UITextField *folderTextEdit;

- (IBAction)createClicked:(id)sender;
- (IBAction)cancelClicked:(id)sender;

@end
