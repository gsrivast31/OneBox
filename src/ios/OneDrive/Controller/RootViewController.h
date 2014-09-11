//
//  RootViewController.h
//  OneDrive
//
//  Created by Vikash Jindal on 18/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TreeNode.h"
#import "PopoverTableViewController.h"
#import "ODCollectionViewController.h"
#import "ODCollectionViewFlowLayout.h"
#import "FileViewController.h"
#import "SearchViewController.h"
#import "SelectionViewController.h"
#import "UploadPopoverTableViewController.h"
#import "AddFolderViewController.h"
#import "MasterDetailViewManager.h"
#include "FileNodeIndex.h"
#include "CommonUIUtils.h"
#include "FavoritesViewController.h"
#include "FileUploadManager.h"

#include <stack>

enum DisplayMode
{
    eGridView = 1,
    eListView,
    eViewCount
};


@interface CustomSearchBarController :UIViewController <UISearchBarDelegate,UIGestureRecognizerDelegate>
{
    UISearchBar *searchBar;
    id parentViewController;
    UIView *popDownView;
}

-(void) performSearch:(SearchType) sType;

@property (strong,nonatomic) UISearchBar *searchBar;
@property (weak,nonatomic) id parentViewController;
@property (strong,nonatomic) UIView *popDownView;;

@end

@interface RootViewController : UIViewController<PopoverTableViewControllerDelegate, UIGestureRecognizerDelegate, ODLayoutDelegate, UITableViewDataSource, UITableViewDelegate, UICollectionViewDataSource, UICollectionViewDelegate,UISearchBarDelegate, AddFolderViewControllerDelegate, UploadPopoverTableViewControllerDelegate, UIImagePickerControllerDelegate, UINavigationControllerDelegate, SearchQueryDelegate>
{
    UIView *containerViewL;
    UIView *containerViewR;
    UIView *dividerView;
    NSArray *horizontalConstraints;
    NSArray *horizontalRightContainerConstraints;
    NSArray *verticalRightContainerConstraints;

    UICollectionView *collectionView;
    FileViewController *fileViewController;
    UIView *fileView;
    CGFloat firstX;
    CGFloat firstY;
    TreeNode *treeNode;
    
    UIPopoverController *addPopover;
    UIPopoverController *folderPopover;
    UIPopoverController *uploadPopover;
    UIPopoverController *pickerPopover;
    UIPopoverController *addFolderPopover;
    
    BOOL isSearchModeActive;
    BOOL isDeletionModeActive;
    UITableView *tableview;
    NSMutableDictionary *accountDictionary;
    TreeNode *selectedNode;
    NSArray *selectedNodeChildren;
    int selectedIndex;
    BOOL isTreeViewVisible;
    IBOutlet UIToolbar *toolbar;
    SearchViewController *searchViewController;
    SelectionViewController *selectionViewController;
    NSString *lastSearch;
    
    CGSize mThumbnailSize;
    
    CommonUIUtils* mCommonUtils;
    FileNode::SharedPtr mSelectedItem;
    FileNode::SharedPtr mQueryNode;
    std::vector<FileNode::SharedPtr> searchChildList;
    std::vector<ODMedia> searchMediaObjs;
    std::stack<FileNode::SharedPtr> folderStack;
    
    NSMutableDictionary *mSyncTimerDict;
    NSMutableDictionary *mSyncRequestPendingDict;
    FileUploadProgressManager::UploadFileProgressManager_t::SharedObserverType_t uploadEventHandler;
}
@property (weak, nonatomic)  CustomSearchBarController *searchBarCtrl;
@property (weak, nonatomic)  IBOutlet UIBarButtonItem *searchButtonView;
@property (weak, nonatomic)  IBOutlet UISearchBar *searchBar;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *folderItem;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *uploadButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *mlistViewButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *mgridViewButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *mSearchButton;
@property (retain, nonatomic) NSString *lastSearch;
@property (retain, nonatomic)  SearchToolBar *mSearchBar;
@property (strong, nonatomic)  FavoritesToolbar *mFavoritesToolBar;
@property (retain, nonatomic) SearchViewController *searchViewController;
@property (retain, nonatomic) SelectionViewController *selectionViewController;
@property (strong,nonatomic) UIPopoverController *searchPopCntr;
@property (nonatomic) DisplayMode displayMode; // Is it list view or Grid View.
@property (nonatomic) NSInteger prevSelectedSortIndex;
@property (nonatomic) NSInteger selectedSortArrangement;
@property (nonatomic, retain) NSMutableArray *toolBarItems;
@property (weak, nonatomic) IBOutlet UISegmentedControl *itemOrderButtonBar;
@property (nonatomic, strong) UIImage *uploadButtonImage;

-(void) addSearchObject:(ODMedia &) mObj;
- (IBAction)search:(id)sender;
-(void) searchQuery:(UITapGestureRecognizer *)gr;
-(void) executeQuery :(NSString *)query;
-(void) reloadQuery;
-(void) textChanged:(NSString *)newText;
- (IBAction)drag:(id)sender;
-(void) resetToolBar : (id) sender;
-(IBAction) showSearchBar:(id) sender;
-(void) showFavoritesBar;
-(void) resetUploadCount;
-(void) uploadAction;

- (IBAction)startSelectionMode:(id)sender;
- (IBAction)folderUpButtonSelected:(id)sender;

- (IBAction)Add:(id)sender;
- (IBAction)ToggleTreeView:(id)sender;
- (IBAction)ChangeFolder:(id)sender;
- (IBAction)MarkFavourite:(id)sender;
- (IBAction)Upload:(id)sender;
- (IBAction)setListView:(id)sender;
- (IBAction)setGridView:(id) sender;

- (IBAction)sortingOrderChanged:(id)sender;

-(void)onSycTimerEvent:(NSTimer *)timer;
-(void)restartSyncTimer: (NSNumber *) accountID;
-(void) setSearchMode: (BOOL) mode;
-(BOOL) getSearchMode;

-(void)pushToCurrentView;
-(void)popFromCurrentView;
-(void)setAccountInView:(NSNumber*)accountID;
-(NSNumber*)getSyncRequestPendingStatus:(NSNumber*)accountID;
-(void)setSyncRequestPendingStatus:(NSNumber*)accountID value:(NSNumber*)status;
@end





