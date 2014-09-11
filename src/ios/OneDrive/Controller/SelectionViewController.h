//
//  SelectionViewController.h
//  OneDriveUI
//
//  Created by Ruchin Kansal on 01/03/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <UIKit/UICollectionViewController.h>
#import "Media.h"
#import "TreeNode.h"
#import "FileNode.h"
#import "PopoverTableViewController.h"
#import "ListViewController.h"

@interface TextView : UIViewController <UITextFieldDelegate>

-(id) initWithText:(NSString*) originalText withFrame:(CGRect)frameRect;
-(void) viewDidLoad;


@property (nonatomic, strong) NSString *inputText;
@property (nonatomic, strong) UITextField *textField;
@end


@interface InputTextToolBar : UIToolbar

-(IBAction) doneClicked:(id)sender;
-(IBAction) cancelClicked:(id)sender;

@property (nonatomic, weak) id sender;
@property (nonatomic, strong) NSString *editedName;
@property (nonatomic, strong) NSString *originalName;

@end



@interface SelectionViewController : UIViewController <UICollectionViewDataSource, UICollectionViewDelegate, PopoverTableViewControllerDelegate, ListViewControllerDelegate>
{
    NSMutableSet *selectionSet;
    UIPopoverController *sharePopover;
    FileNode::SharedPtr mQueryNode;
    std::vector<FileNode::SharedPtr> searchChildList;
}

- (IBAction)renameSelectedFiles:(id)sender;
- (IBAction)deleteSelectedItems:(id)sender;
- (IBAction)shareSelectedFiles:(id)sender;
- (IBAction)markFavoriteSelectedFiles:(id)sender;
- (IBAction)rename:(id)sender;

-(void) enableDisableToolBarItems;
-(void) updateSelectionNode:(std::vector<ODMedia> &) mediaList;
-(void) setSelectedNode:(FileNode::SharedPtr) selectedNode;

- (void) dismissEditView;
- (void) updateItemName: (NSString*) newName;
- (void) callBackFromListViewController:(ODMedia)parent forWorkflow:(NSInteger)workflow;

@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (strong, nonatomic) IBOutlet UIToolbar *selectionToolbar;
@property (nonatomic, strong) NSMutableArray *toolBarItems;
@property (nonatomic,strong) InputTextToolBar *textToolbar;
@property (nonatomic, strong) TextView *textView;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *renameButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *favoriteButton;
- (IBAction)backButtonHandler:(id)sender;
- (id)initWithFrame:(CGRect)frame;
- (void)viewDidLoad;
-(void)initLayout:(BOOL)searchMode;
@end



