//
//  FileViewController.h
//  OneDriveUI
//
//  Created by Vikash Jindal on 08/01/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "PopoverTableViewController.h"
#include "AppController.h"
#include "FileNodeIndex.h"
#import "SelectionViewController.h"
#import "ListViewController.h"

@interface FileViewController : UIViewController<UIWebViewDelegate, PopoverTableViewControllerDelegate,
                                ListViewControllerDelegate, UIAlertViewDelegate>
{
    FileNodeIndex::SharedPtr currentNodeIndex;
    FileNode::SharedPtr currentNode;
    FileNode::SharedPtr selectedNode;
    
    //FileNode::ChildrenContainer selectedNodeChildren;
    
    UIPopoverController *optionsPopup;
    UIPopoverController *infoPopup;
    NSString* downloadedPath;
    AppController::FileDownloadManager_t::SharedObserverType_t downloadEventHandler;
}
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *actionButton;
@property (weak, nonatomic) IBOutlet UILabel *fileNameLabel;
- (IBAction)showOptions:(id)sender;
- (IBAction)markFavorite:(id)sender;
- (IBAction)renameFile:(id)sender;
- (IBAction)deleteItem:(id)sender;

- (void) dismissEditView;
- (void) updateItemName: (NSString*) newName;
- (void) callBackFromListViewController:(ODMedia)parent forWorkflow:(NSInteger)workflow;

@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (nonatomic, strong) UIDocumentInteractionController *docInteractionController;
@property (weak, nonatomic) IBOutlet UIWebView *webView;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *favoriteButton;
@property (nonatomic) ID selectedNodeID;
@property (strong, nonatomic) IBOutlet UIToolbar *selectionToolbar;
@property (nonatomic, strong) NSMutableArray *toolBarItems;
@property (nonatomic,strong) InputTextToolBar *textToolbar;
@property (nonatomic, strong) TextView *textView;
@property (nonatomic) NSInteger currentIndex;
@property (nonatomic) NSInteger currentAccount;
-(void) reload;
@end
