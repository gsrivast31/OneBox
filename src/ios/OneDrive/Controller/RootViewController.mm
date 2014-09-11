//
//  RootViewController.m
//  OneDrive
//
//  Created by Vikash Jindal on 18/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "RootViewController.h"
#import "TreeViewCell.h"
#import "AddAccountViewController.h"
#import "WrappedAccountData.h"
#import "Account.h"
#import "Media.h"
#import "IconCell.h"
#import "PopupBase.h"
#include "CommonUtils.h"
#include "AccountServiceController.h"
#include "AppController.h"
#include "ODDataController.h"
#include "ODLogger.h"
#include "UString.h"
#import "WrappedMediaData.h"
#import "MediaAttribute.h"
#import "AddFolderViewController.h"
#import "CommonUIUtils.h"
#import "FileUploadManager.h"
#import <AssetsLibrary/AssetsLibrary.h>

#define DIVIDER_WIDTH 25

const UString kDummyNode = "dummyNode";

@implementation CustomSearchBarController

@synthesize searchBar;
@synthesize popDownView;

-(id) initSearchCtrl: (CGRect)frame parentController:(UIViewController*)parentController dropdownImage:(UIImage*)dropdownImage
{
    self = [super init];
    self.parentViewController = parentController;
    self.searchBar = [[UISearchBar alloc] initWithFrame:frame];
    [self.searchBar setImage:dropdownImage forSearchBarIcon:UISearchBarIconSearch state:UIControlStateNormal];
    CGRect popDownRect = CGRectMake(0, 0, 15, self.searchBar.frame.size.height);
    self.popDownView = [[UIView alloc] initWithFrame:popDownRect];
    self.searchBar.delegate = self ;
    
    UITapGestureRecognizer *singleFingerTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self
                                            action:@selector(handleSingleTap:)];
    [self.view addGestureRecognizer:singleFingerTap];
    singleFingerTap.delegate = self;
    return self;
}

-(void) viewDidLoad
{
    [self.view addSubview:self.searchBar];
    [self.view addSubview:self.popDownView];
}

- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText
{
  //  [(RootViewController *)self.parentViewController textChanged:searchText];
}

-(void) performSearch:(SearchType) sType
{
    
}

//The event handling method
- (void)handleSingleTap:(UITapGestureRecognizer *)recognizer {
    CGPoint location = [recognizer locationInView:[recognizer.view superview]];
    
    //Do stuff here...
}

@end


@interface RootViewController ()
-(void) getAllAccount:(ServiceProvider) serviceID;
-(void) FetchAccount:(NSNotification *) notification;
-(void) reloadData;
@end

@implementation RootViewController

#pragma mark -
#pragma mark SubstitutableDetailViewController

// -------------------------------------------------------------------------------
//	setNavigationPaneBarButtonItem:
//  Custom implementation for the navigationPaneBarButtonItem setter.
//  In addition to updating the _navigationPaneBarButtonItem ivar, it
//  reconfigures the toolbar to either show or hide the
//  navigationPaneBarButtonItem.
// -------------------------------------------------------------------------------
- (void)setNavigationPaneBarButtonItem:(UIBarButtonItem *)navigationPaneBarButtonItem
{
}

@synthesize searchViewController;
@synthesize selectionViewController;
@synthesize  lastSearch;
@synthesize folderItem;
@synthesize uploadButton;
@synthesize displayMode;
@synthesize mSearchBar;
@synthesize mFavoritesToolBar;
@synthesize toolBarItems;
@synthesize prevSelectedSortIndex;
@synthesize selectedSortArrangement;
@synthesize uploadButtonImage;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
     //Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    mSyncTimerDict = [[NSMutableDictionary alloc] init];
    mSyncRequestPendingDict = [[NSMutableDictionary alloc] init];
    isSearchModeActive = NO;
    mCommonUtils = [CommonUIUtils getInstance];
    mThumbnailSize = CGSizeMake(96 , 96);
	// Do any additional setup after loading the view.
    containerViewL = [[UIView alloc] init];
    containerViewR = [[UIView alloc] init];
    dividerView = [[UIView alloc] init];
    [self.view addSubview:containerViewL];
    [self.view addSubview:containerViewR];
    [self.view addSubview:dividerView];
    self.displayMode = eGridView; // 1 stands for listview, 0 stands for grid view.
    self.prevSelectedSortIndex = 0;
    self.selectedSortArrangement = 0;
    
    dividerView.backgroundColor = [UIColor grayColor];
    [containerViewL setTranslatesAutoresizingMaskIntoConstraints:NO];
    [containerViewR setTranslatesAutoresizingMaskIntoConstraints:NO];
    [dividerView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    dividerView.userInteractionEnabled = YES;
    UIPanGestureRecognizer *panGestureRecognizer = [[UIPanGestureRecognizer alloc]
                                   initWithTarget:self action:@selector(drag:)];
    
    [dividerView addGestureRecognizer:panGestureRecognizer];
    ODCollectionViewController *odODCollectionViewController = [[ODCollectionViewController alloc] initWithCollectionViewLayout:[[ODCollectionViewFlowLayout alloc] init]];
    collectionView = odODCollectionViewController.collectionView;
    collectionView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.jpg"]];
    NSDictionary *views = NSDictionaryOfVariableBindings(containerViewL, dividerView,containerViewR, toolbar,collectionView);
    //NSDictionary *metrics = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithDouble:DIVIDER_WIDTH], @"DIVIDER_WIDTH", nil];
    //horizontalConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[containerViewL(==300)][dividerView(==DIVIDER_WIDTH)][containerViewR(>=0)]|" options:0 metrics:metrics views:NSDictionaryOfVariableBindings(containerViewL, dividerView, containerViewR)];
    
    horizontalConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[containerViewL(==0)][dividerView(==0)][containerViewR(>=0)]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(containerViewL, dividerView, containerViewR)];
    
    [self.view addConstraints:horizontalConstraints];
    isTreeViewVisible = false;
    UIImage *ddImage = [UIImage imageNamed:@"button-play-mini.png"];
    self.searchBar.delegate = self;
    UITapGestureRecognizer *searchTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(searchQuery:)];
    searchTap.delegate = self;
    [self.searchBar addGestureRecognizer:searchTap];
    
    
    [self.searchBar setImage:ddImage forSearchBarIcon:UISearchBarIconSearch state:UIControlStateNormal];
#if 0 
    // For the time being keeping this code. In case we need this.
    self.searchBarCtrl = [[CustomSearchBarController alloc] initSearchCtrl:searchRect parentController:self dropdownImage:ddImage];
    [toolbar addSubview:self.searchBarCtrl.view];
#endif
    [self.view addConstraints:horizontalConstraints];
    self.lastSearch = [[NSString alloc] init] ;
    
         
    NSArray *vConstrainsts = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[toolbar][containerViewL]|" options:0 metrics:nil views:views];
    [self.view addConstraints:vConstrainsts];
    
    vConstrainsts = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[toolbar][containerViewR]|" options:0 metrics:nil views:views];
    [self.view addConstraints:vConstrainsts];
    
    vConstrainsts = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[toolbar][dividerView]|" options:0 metrics:nil views:views];
    [self.view addConstraints:vConstrainsts];
    tableview = [[UITableView alloc] init];
    [containerViewL addSubview:tableview];
    tableview.dataSource = self;
    tableview.delegate = self;
    [tableview setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    collectionView.dataSource = self;
    collectionView.delegate = self;
    [containerViewR addSubview:collectionView];
    
    self.searchViewController = [[SearchViewController alloc] initSearchView:collectionView.frame];
    
    [self.searchViewController collectionView].delegate = self.searchViewController;
    [self.searchViewController collectionView].dataSource = self.searchViewController;
    [containerViewR addSubview:[self.searchViewController collectionView]];
    [[self.searchViewController collectionView] setHidden:TRUE];
    
    fileViewController = [self.storyboard instantiateViewControllerWithIdentifier:@"fullScreenView"];
    /*fileView = fileViewController.view;
    [self addChildViewController:fileViewController];
    [fileView setTranslatesAutoresizingMaskIntoConstraints:NO];*/


    
    
    [collectionView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [tableview setTranslatesAutoresizingMaskIntoConstraints:NO];
    [containerViewL addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[tableview]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(tableview)]];
    [containerViewL addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[tableview]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(tableview)]];
    horizontalRightContainerConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[collectionView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(collectionView)];
    containerViewR.autoresizesSubviews = TRUE;
    [containerViewR addConstraints:horizontalRightContainerConstraints];
    verticalRightContainerConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[collectionView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(collectionView)];
    [containerViewR addConstraints:verticalRightContainerConstraints];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(FetchAccount:) name:@"AccountFetched" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(ProcessMedia:) name:@"MediaNotification" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(ProcessMediaUpdate:) name:@"MediaUpdateNotification" object:nil];
    UINib *cellNib = [UINib nibWithNibName:@"IconCell" bundle:nil];
    UINib *listViewcellNib = [UINib nibWithNibName:@"listViewCell" bundle:nil];
    [collectionView registerNib:cellNib forCellWithReuseIdentifier:@"ICON"];
    [collectionView registerNib:listViewcellNib forCellWithReuseIdentifier:@"listCell"];

    listViewcellNib = [UINib nibWithNibName:@"listViewCell_landcape" bundle:nil];
    [collectionView registerNib:listViewcellNib forCellWithReuseIdentifier:@"listCell_landscape"];
    
    UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(activateDeletionMode:)];
    longPress.delegate = self;
    [collectionView addGestureRecognizer:longPress];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(endDeletionMode:)];
    tap.delegate = self;
    [collectionView addGestureRecognizer:tap];

    accountDictionary = [[NSMutableDictionary alloc] initWithCapacity:1];
    
    isTreeViewVisible = false;
 
	// Do any additional setup after loading the view.
    
    //get all suppored services.
    
    //get all acconts for each services in loop
    [self getAllAccount:kSPDropbox];
    [self getAllAccount:kSPSkyDrive];
    [self getAllAccount:kSPGDrive];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(dismiss:) name:@"AccountSelectedInMaster" object:nil];
    
     self.mSearchBar =  [[[NSBundle mainBundle] loadNibNamed:@"searchToolbar" owner:self options:nil] objectAtIndex:0];
    self.mSearchBar.frame = self.navigationController.toolbar.frame;
    self.mSearchBar.sender = self;
    self.mSearchBar.mSearchBar.delegate = self ;
    
    self.mFavoritesToolBar = [[[NSBundle mainBundle] loadNibNamed:@"FavoritesToolbar" owner:self options:nil] objectAtIndex:0];
    
    self.mFavoritesToolBar.frame = self.navigationController.toolbar.frame;
    self.mFavoritesToolBar.sender = self;
    
    toolBarItems = [toolbar.items mutableCopy];
    mQueryNode.reset(new FileNode(ODMedia(),FileNode::SharedPtr()));
    [self.itemOrderButtonBar setImage:[UIImage imageNamed:@"arrow_down.png"] forSegmentAtIndex:self.prevSelectedSortIndex];
    
    
    uploadEventHandler = FileUploadProgressManager::instance().GetUploadNotificationManager()->MakeShared([self](FileUploadProgressManager::UploadInfoData_t uploadInfo){
        
        [self performSelectorOnMainThread:@selector(resetUploadCount) withObject:nil waitUntilDone:YES];
        
    //    [self resetUploadCount];
        
    });
    FileUploadProgressManager::instance().AddObserver(uploadEventHandler);

    
    
}


- (BOOL)shouldAutomaticallyForwardRotationMethods
{
    return YES;
}
- (BOOL)shouldAutomaticallyForwardAppearanceMethods
{
    return YES;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.

}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AccountFetched" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"MediaNotification" object:nil];
    
}

#pragma mark -
#pragma mark Table view data source

// Customize the number of sections in the table view.
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [treeNode descendantCount];
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *CellIdentifier = @"Cell";
    TreeNode *node = [[treeNode flattenElements] objectAtIndex:indexPath.row + 1];
    
    TreeViewCell *cell = [[TreeViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                           reuseIdentifier:CellIdentifier
                           level:[node levelDepth] - 1
                           expanded:node.inclusive];
    cell.valueLabel.text = node.value;
    return cell;
}

#pragma mark - data source methods
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 1;
}
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    if([self getSearchMode] == YES)
    {
        FileNode::AddFileNodes(searchChildList, searchMediaObjs);
        FileNode::UpdatFileNodeWithCustomChildren(mQueryNode, searchChildList);
    }
    return [self getSearchMode] == YES ? mQueryNode->getChildren().size() : mSelectedItem ? mSelectedItem->getChildren().size() :0;
}
- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView1 cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    
    auto selectedItem = [self getSearchMode] == YES ? mQueryNode : mSelectedItem;
   
    UICollectionViewCell *icon ;
    if(self.displayMode == eGridView)
        icon = [collectionView dequeueReusableCellWithReuseIdentifier:@"ICON" forIndexPath:indexPath];
    else
    {
        NSString *identifier = @"listCell";
        
        if([[UIDevice currentDevice] orientation] ==  UIDeviceOrientationLandscapeLeft ||
          [[UIDevice currentDevice] orientation] ==  UIDeviceOrientationLandscapeRight)
           identifier =  @"listCell_landscape";
        icon = [collectionView dequeueReusableCellWithReuseIdentifier:identifier forIndexPath:indexPath];
    }
    
    auto node = selectedItem->GetChild(indexPath.row);
    if(!node.get())
        return nil;
    auto media = node->GetMedia();
    
    if(self.displayMode == eGridView)
    {
        IconCell *iCell = (IconCell*)(icon);
        
        iCell.label.text = node->GetName().getPlatformString();
        iCell.label.textColor = [UIColor whiteColor];
    }
    else
    {
        listViewCell *lCell = (listViewCell *)(icon);
		lCell.name.text = node->GetName().getPlatformString();
        lCell.name.textColor = [UIColor whiteColor];
        
            if(!media.is_folder && !node->getChildren().size())
            {
                 lCell.size.textAlignment = NSTextAlignmentLeft;
                
                NSString *units = @"B";
                
                double mediaSize = media.size;
                if(mediaSize > 1000)
                {
                    mediaSize /= 1000;
                    units = @"KB";
                }
                if(mediaSize > 1000)
                {
                    mediaSize /= 1000;
                    units = @"MB";
                }
                if(mediaSize > 1000)
                {
                    mediaSize /= 1000;
                    units = @"GB";
                }
                
                lCell.size.text = [NSString stringWithFormat:@"%.1f %@",mediaSize,units];
                lCell.size.textColor = [UIColor whiteColor];
            }
            else
                lCell.size.text = @"";
            lCell.date.text = media.mod_date.ToString().getPlatformString();
            lCell.date.textColor = [UIColor whiteColor];

        }
   
            UIImage* image = [UIImage imageNamed:media.icon.getPlatformString()];
            if (image == nil) {
                image = [UIImage imageWithContentsOfFile:media.icon.getPlatformString()];
            }
            if(self.displayMode == eGridView)
            {
                IconCell *iCell = (IconCell*)(icon);
                iCell.thumbnail.image = image ;
            }
            else
            {
                listViewCell *lCell = (listViewCell *)(icon);
                lCell.thumbnail.image = image ;
            }
     
    if(self.displayMode == eGridView)
    {
        IconCell *icell = (IconCell*)icon;
        [icell.deleteButton addTarget:self action:@selector(delete:) forControlEvents:UIControlEventTouchUpInside];
    }
    
    // Add UploadView if needed.
#if 0
    // TODO: Will enable this code once Backend for tracking fileupload progress is complete.
    //Switching it for now.
    if(self.displayMode == eGridView && FileUploadProgressManager::instance().IsUploading(media.media_id))
    {
        IconCell *iCell = (IconCell*)(icon);
        [iCell showIndicator];
                   
    }
    
#endif
    return icon;
}



#pragma mark - delete for button
- (void)delete:(UIButton *)sender
{
    NSIndexPath *indexPath ;
    if(self.displayMode == eGridView)
        indexPath = [collectionView indexPathForCell:(IconCell *)sender.superview.superview];
    else if(self.displayMode == eListView)
        indexPath = [collectionView indexPathForCell:(listViewCell *)sender.superview.superview];
        
    [collectionView deleteItemsAtIndexPaths:[NSArray arrayWithObject:indexPath]];
}

//
#pragma mark - delegate methods
- (void)collectionView:(UICollectionView *)collectionView1 didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    auto selectedItem = [self getSearchMode] == YES ? mQueryNode->GetChild(indexPath.row) : mSelectedItem->GetChild(indexPath.row);
    auto nodeMedia = selectedItem->GetMedia();
    
   
    if(selectedItem->GetParent())
    {
            if(nodeMedia.is_folder || selectedItem->getChildren().size())
            {
                [self pushToCurrentView];
                
                // If we are in search mode, then find the true selected folder and select that
                if([self getSearchMode] == YES)
                {
                    auto fileNodeIndex = FileNodeIndex::GetFileNodeIndex(nodeMedia.account_id);
                    mSelectedItem = fileNodeIndex->GetNode(nodeMedia.media_id);
                    self.title = selectedItem->GetName().getPlatformString();
                    folderItem.title = selectedItem->GetName().getPlatformString();
                    // remove searchMode.
                    [self resetToolBar:self];
                    
                    return;

                }
                else
                    mSelectedItem = selectedItem;
                self.title = selectedItem->GetName().getPlatformString();
                folderItem.title = selectedItem->GetName().getPlatformString();
                [collectionView reloadData];

            }
            else{
                auto media = mSelectedItem->GetMedia();
                fileViewController.selectedNodeID = media.media_id;
                fileViewController.currentAccount = [[mCommonUtils getActiveAccountID] intValue];
                fileViewController.currentIndex = indexPath.row;
                [self.navigationController pushViewController:fileViewController animated:NO];
            }
            
    }
}

- (BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    if (isDeletionModeActive) return NO;
    else return YES;
}


- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if(segue.identifier)
    {
        if([segue.identifier isEqualToString:@"AddAccountSegue"])
        {
            AddAccountViewController *targetVC = (AddAccountViewController*)segue.destinationViewController;
            targetVC.name = @"TEST";
            targetVC.serviceID = kSPDropbox;
        }
        else if([segue.identifier isEqualToString:@"SelectionViewSegue"])
        {
            SelectionViewController *target = segue.destinationViewController;
            
            if([self getSearchMode] == YES)
            {
                [target updateSelectionNode:searchMediaObjs];
            }
            else
                [target setSelectedNode:mSelectedItem];
            
        }
    }
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    //TODO : (abedi) Need to figure out a way to do it in a lighter way.
    [collectionView reloadData];
}

- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
}

-(IBAction) showSearchBar:(id) sender
{
    self.mSearchBar.mSearchBar.hidden = NO;
    [toolbar setItems:[NSMutableArray arrayWithArray:mSearchBar.items] animated:YES];
    [mSearchBar setFocusOnSearch];

}

-(void) resetUploadCount
{
    // Get the uploadButtonImage
    // Make sure this always run in the main thread, otherwise it might result in UI hang.
    
    UIImage *upldImg = [UIImage imageNamed:@"cloud-upload-mini.png"];
    NSString *intString = [NSString stringWithFormat:@"%d", FileUploadProgressManager::instance().TotalMediaUploading()];
    if(FileUploadProgressManager::instance().TotalMediaUploading() <= 0)
        intString = @" ";

    uploadButtonImage = [ImageUtils drawText:intString inImage:upldImg
                                        atPoint:CGPointMake(upldImg.size.width, 0)
                                        offset:CGPointMake(10, 5) ];
    UIButton *customButton = [UIButton buttonWithType:UIButtonTypeCustom];
    customButton.bounds = CGRectMake( 0, 0, uploadButtonImage.size.width, uploadButtonImage.size.height );
    [customButton setImage:uploadButtonImage forState:UIControlStateNormal];
    [customButton addTarget:self action:@selector(uploadAction) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *customBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:customButton];
    [self.toolBarItems replaceObjectAtIndex:1 withObject:customBarButtonItem];
    [toolbar setItems:[NSMutableArray arrayWithArray:self.toolBarItems] animated:YES]; // Reset the search bar.
   
}

-(void) uploadAction
{
    [self Upload:self];
}

-(void) setSearchMode:(BOOL)mode
{
    isSearchModeActive = mode;
}

-(BOOL) getSearchMode
{
    return isSearchModeActive || searchMediaObjs.size() > 0 ||  ![self.lastSearch isEqual: @""];
}

-(void) showFavoritesBar
{
    [self setSearchMode:YES];
    [toolbar setItems:[NSMutableArray arrayWithArray:mFavoritesToolBar.items] animated:YES];
}


-(void) resetToolBar : (id) sender
{
 
    [self setSearchMode:NO];
    [self.mSearchBar.mSearchBar setText:@""];
    [toolbar setItems:[NSMutableArray arrayWithArray:self.toolBarItems] animated:YES]; // Reset the search bar.
    self.lastSearch =@"";
    
    searchMediaObjs.clear();

    [collectionView reloadData];

}

- (IBAction)drag:(UIPanGestureRecognizer*)sender {
    [self.view removeConstraints:horizontalConstraints];
    
    CGPoint translatedPoint = [(UIPanGestureRecognizer*)sender translationInView:self.view];
    
    if([(UIPanGestureRecognizer*)sender state] == UIGestureRecognizerStateBegan) {
        
        firstX = [[sender view] center].x;
        firstY = [[sender view] center].y;
    }
    
    translatedPoint = CGPointMake(firstX+translatedPoint.x, firstY);
    float dividerViewWidth = 0.0f;
    
    float conatinerLWidth = 0.0f;
    if (translatedPoint.x > self.view.frame.size.width)
    {
        conatinerLWidth = self.view.frame.size.width;
        dividerViewWidth = 0.0;
    }
    else if (translatedPoint.x < 0)
    {
        conatinerLWidth = 0;
        dividerViewWidth = 0.0;
        isTreeViewVisible = false;

    }
    else if ((translatedPoint.x + DIVIDER_WIDTH) > self.view.frame.size.width)
    {
        conatinerLWidth = self.view.frame.size.width - DIVIDER_WIDTH;
        dividerViewWidth = DIVIDER_WIDTH;
        isTreeViewVisible = true;
    }
    else
    {
        conatinerLWidth = translatedPoint.x;
        dividerViewWidth = DIVIDER_WIDTH;
        isTreeViewVisible = true;
    }
    
    NSDictionary *metrics = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithDouble:conatinerLWidth], @"w1", [NSNumber numberWithDouble:dividerViewWidth], @"w2", nil];
    
    
    NSDictionary *views = NSDictionaryOfVariableBindings(containerViewL, dividerView, containerViewR);
    horizontalConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[containerViewL(==w1@400)][dividerView(==w2)][containerViewR(>=96@1000)]|" options:0 metrics:metrics views:views];
    
    [self.view addConstraints:horizontalConstraints];
    
}

- (IBAction)startSelectionMode:(id)sender {
}

- (IBAction)folderUpButtonSelected:(id)sender {
    if(selectedNode.parent){
        selectedNode = selectedNode.parent;
        selectedNodeChildren = [selectedNode.children allKeys];
        folderItem.title = selectedNode.value;
        [collectionView reloadData];
    }
}

- (IBAction)Add:(id)sender {
    if(addPopover == nil)
    {
        PopoverTableViewController *addOption = [[PopoverTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
        addOption.delegate = self;
        addOption.selected = -1;
        
        [addOption initSectionArray:1];
        PopupBase* item1 = [[PopupBase alloc] init];
        item1.index = 0;
        item1.name = @"DropBox";
        item1.image = [[CommonUIUtils getInstance] getServiceIcon:kSPDropbox];
        [addOption addToArray:0 objectToAdd:item1];
        
        PopupBase* item2 = [[PopupBase alloc] init];
        item2.index = 1;
        item2.name = @"Google Drive";
        item2.image = [[CommonUIUtils getInstance] getServiceIcon:kSPGDrive];
        [addOption addToArray:0 objectToAdd:item2];

        PopupBase* item3 = [[PopupBase alloc] init];
        item3.index = 2;
        item3.name = @"SkyDrive";
        item3.image = [[CommonUIUtils getInstance] getServiceIcon:kSPSkyDrive];
        [addOption addToArray:0 objectToAdd:item3];

        addPopover = [[UIPopoverController alloc]initWithContentViewController:addOption] ;
    }
    if([addPopover isPopoverVisible])
        [addPopover dismissPopoverAnimated:YES];
    else
        [addPopover presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];

}


- (IBAction)ToggleTreeView:(id)sender {
    [self.view removeConstraints:horizontalConstraints];

    isTreeViewVisible = !isTreeViewVisible;
    if(isTreeViewVisible)
    {
       NSDictionary *metrics = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithDouble:DIVIDER_WIDTH], @"DIVIDER_WIDTH", nil]; 
        horizontalConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[containerViewL(==300)][dividerView(==DIVIDER_WIDTH)][containerViewR(>=0)]|" options:0 metrics:metrics views:NSDictionaryOfVariableBindings(containerViewL, dividerView, containerViewR)];
        [self.view addConstraints:horizontalConstraints];
    }
    else
    {
        
        horizontalConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[containerViewL(==0)][dividerView(==0)][containerViewR(>=0)]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(containerViewL, dividerView, containerViewR)];
        [self.view addConstraints:horizontalConstraints];
        
    }
    
}

- (IBAction)MarkFavourite:(id)sender {
}

- (IBAction)Upload:(id)sender {
    //dismiss popovers
	if(uploadPopover && uploadPopover.popoverVisible)
		[uploadPopover dismissPopoverAnimated:NO];
	
    if(pickerPopover && pickerPopover.popoverVisible)
		[pickerPopover dismissPopoverAnimated:NO];
	
	UploadPopoverTableViewController *uploadController = [[UploadPopoverTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
    uploadController.delegate = self;
    uploadController.folderName = folderItem.title;
    //for providing navigation to sub views in settings dialog
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:uploadController];
    navigationController.title = @"Settings";
    //navigationController.navigationBarHidden = YES;
    navigationController.delegate = uploadController;
    
	if(!uploadPopover) {
        uploadPopover = [[UIPopoverController alloc] initWithContentViewController:navigationController];
    }
    else {
        // Setup the popover for use from the navigation bar.
        uploadPopover.contentViewController = navigationController;
    }
    
    uploadPopover.popoverContentSize = CGSizeMake(uploadController.view.frame.size.width, uploadController.view.frame.size.height);
    
  //  uploadButton = sender;
	
    [uploadPopover presentPopoverFromBarButtonItem:[self.toolBarItems objectAtIndex:1] permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

void callbackForCreatingFolder (bool success, ODMedia media)
{
    
}

void CallbackForUpload(UString localPath, int64_t finished, bool success)
{
    //TODO: Fix this.
    FileUploadProgressManager::instance().updateStatus(localPath, finished, success ? UploadInfo::kUploadCompleted:UploadInfo::kUploadFailed);
    
}

- (void) UploadFileInCurrentService:(NSURL *)fileURL
{
    AppController &applicationCtr = AppController::instance();
    NSString* fileName = [fileURL lastPathComponent];
    auto media = mSelectedItem->GetMedia();
    /*NSString* path = media.full_path.getPlatformString();
    path = [path stringByAppendingString:@"/"];
    path = [path stringByAppendingString:fileName];*/
    NSString* localPath = fileURL.path;
    UString uPath = localPath;
    FileUploadProgressManager::instance().AddMediaForUploading(uPath);
    applicationCtr.UploadFile(media.media_id ,media.account_id, localPath, media.file_id, CallbackForUpload);
    
}

- (void) openGallery
{
    [uploadPopover dismissPopoverAnimated:YES];
    UIImagePickerController* picker = [[UIImagePickerController alloc] init];
    picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    picker.delegate = self;
    
    pickerPopover = [[UIPopoverController alloc] initWithContentViewController:picker];
    [pickerPopover presentPopoverFromBarButtonItem:[self.toolBarItems objectAtIndex:1] permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

- (void) openCamera
{
    [uploadPopover dismissPopoverAnimated:YES];
    UIImagePickerController* picker = [[UIImagePickerController alloc] init];
   // picker.sourceType = UIImagePickerControllerCameraCaptureModePhoto;
    picker.delegate = self;
    
    pickerPopover = [[UIPopoverController alloc] initWithContentViewController:picker];
    [pickerPopover presentPopoverFromBarButtonItem:[self.toolBarItems objectAtIndex:1] permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}


#pragma mark -
#pragma mark Display View

- (IBAction)setListView:(id)sender{

    self.displayMode = eListView;
    ODCollectionViewFlowLayout  *layout = (ODCollectionViewFlowLayout *)collectionView.collectionViewLayout;
    [layout invalidateLayout];
    [collectionView reloadData];
}

- (IBAction)setGridView:(id)sender{
    
    self.displayMode = eGridView;
    ODCollectionViewFlowLayout  *layout = (ODCollectionViewFlowLayout *)collectionView.collectionViewLayout;
    [layout invalidateLayout];
    [collectionView reloadData];
}

- (IBAction)sortingOrderChanged:(id)sender {
    auto fileNodeIndex = FileNodeIndex::GetFileNodeIndex([[mCommonUtils getActiveAccountID] intValue]);
    auto fileNode = fileNodeIndex->GetRootNode();
    NSInteger selectedSortIndex = self.itemOrderButtonBar.selectedSegmentIndex;
    if (prevSelectedSortIndex == selectedSortIndex) {
        selectedSortArrangement = !selectedSortArrangement;
        if (selectedSortArrangement == 0) 
            [self.itemOrderButtonBar setImage:[UIImage imageNamed:@"arrow_down.png"] forSegmentAtIndex:selectedSortIndex];
        else
            [self.itemOrderButtonBar setImage:[UIImage imageNamed:@"arrow_up.png"] forSegmentAtIndex:selectedSortIndex];
    }
    else {
        [self.itemOrderButtonBar setImage:nil forSegmentAtIndex:prevSelectedSortIndex];
        [self.itemOrderButtonBar setImage:[UIImage imageNamed:@"arrow_down.png"] forSegmentAtIndex:selectedSortIndex];
        prevSelectedSortIndex = selectedSortIndex;
        selectedSortArrangement = 0;
    }
    

    switch(self.itemOrderButtonBar.selectedSegmentIndex){
        case 0:
            fileNode->ChangeChildrenOrder(kOrderByName, selectedSortArrangement, true);
            break;
        case 1:
            fileNode->ChangeChildrenOrder(kOrderByDate, selectedSortArrangement, true);
            break;
        case 2:
            fileNode->ChangeChildrenOrder(kOrderBySize, selectedSortArrangement, true);
            break;
        case 3:
            fileNode->ChangeChildrenOrder(kOrderByType, selectedSortArrangement, true);
            break;
    }
    [collectionView reloadData];
}


#pragma mark -
#pragma mark UIImagePickerControllerDelegate

// this get called when an image has been chosen from the library or taken from the camera
//
- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    [pickerPopover dismissPopoverAnimated:YES];
    NSURL* resourceURL = [info objectForKey:UIImagePickerControllerMediaURL];
    if (resourceURL) {
    }
    else {
        resourceURL = [info objectForKey:UIImagePickerControllerReferenceURL];
        ALAssetsLibrary *assetLibrary = [ALAssetsLibrary new];
        [assetLibrary assetForURL:resourceURL
                      resultBlock:^(ALAsset *asset) {
                          // get data
                          ALAssetRepresentation *assetRep = [asset defaultRepresentation];
                          CGImageRef cgImg = [assetRep fullResolutionImage];
                          NSString *fileName = [assetRep filename];
                          NSLog(@"%@", fileName);
                          UIImage *img = [UIImage imageWithCGImage:cgImg];
                          NSData *data = UIImagePNGRepresentation(img);
                          NSString *cacheDir = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES)lastObject];
                          NSURL *tempFileURL = [NSURL fileURLWithPath:[cacheDir stringByAppendingPathComponent:fileName]];
                          BOOL result = [data writeToFile:tempFileURL.path atomically:YES];
                          if (result) {
                              [self UploadFileInCurrentService:tempFileURL];
                          }
                      }
                      failureBlock:^(NSError *error) {
                          NSLog(@"%@", error);
                      }];
    }
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [pickerPopover dismissPopoverAnimated:YES];
}

- (void) openAddFolderWorkflow
{
    [uploadPopover dismissPopoverAnimated:NO];
    AddFolderViewController *contentController = [[AddFolderViewController alloc] initWithNibName:@"AddFolderViewController" bundle:nil];
    contentController.delegate = self;
    if(!addFolderPopover) {
        addFolderPopover = [[UIPopoverController alloc] initWithContentViewController:contentController];
    }
    else {
        // Setup the popover for use from the navigation bar.
        addFolderPopover.contentViewController = contentController;
    }
    
    addFolderPopover.popoverContentSize = CGSizeMake(contentController.view.frame.size.width, contentController.view.frame.size.height);
    
    [addFolderPopover presentPopoverFromBarButtonItem:[self.toolBarItems objectAtIndex:1] permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

- (void) removePopovers
{
    if(addFolderPopover && addFolderPopover.popoverVisible)
		[addFolderPopover dismissPopoverAnimated:YES];
}

- (void) createFolder:(NSString *)folderName
{
    [self removePopovers];
    AppController &applicationCtr = AppController::instance();
    auto media = mSelectedItem->GetMedia();
    /*NSString* path = media.full_path.getPlatformString();
    path = [path stringByAppendingString:@"/"];
    path = [path stringByAppendingString:folderName];*/
    applicationCtr.CreateFolder(media.account_id, media.file_id, folderName, callbackForCreatingFolder);
}

- (void) copyFromClipboard
{
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    UIPasteboard *clipboard = [pUtils getClipboard];
    if (clipboard) {
        bool copy = true;
        if ([clipboard.name compare:kClipBoardForCut] == NSOrderedSame) {
            copy = false;
        }

        NSArray *strArray = clipboard.strings;
        for (NSString* file in strArray) {
            AppController &applicationCtr = AppController::instance();
            
            UString fileName;
            UString filePath;
            Utils::SplitFilePathAndName(file, filePath, fileName);
            
            Media* media = [selectedNode getNodeMedia];
            NSString* path = media.full_path;
            path = [path stringByAppendingString:@"/"];
            path = [path stringByAppendingString:fileName.getPlatformString()];
            
            if (copy) {
                //applicationCtr.CopyFile(media.account_id, file, path, CallbackForUpload);
            }
            else {
                //applicationCtr.MoveFile(media.account_id, file, path, CallbackForUpload);
            }
        }
    }
}

- (IBAction)RefreshView:(id)sender {
}

- (void) takenInputAtIndex:(UITableViewController *)popover selectedIndex:(NSInteger) index{
    
    if(popover == addPopover.contentViewController){
        [addPopover dismissPopoverAnimated:YES];
        [self performSegueWithIdentifier:@"AddAccountSegue" sender:self];
    }
    else if(popover == self.searchPopCntr.contentViewController)
    {
        [self.searchPopCntr dismissPopoverAnimated:YES];
        NSString* query;
        switch (index) {
            case 0:
                query = @"Images";
                break;
            case 1:
                query = @"Videos";
                break;
            case 2:
                query = @"PDF";
                break;
            case 3:
                query = @"Documents";
                break;
            case 4:
                query = @"Other";
                break;
            default:
                break;
        }
        [self executeQuery:query];
    }
}

void CallbackAccountForService(RESULT res, ODAccount account)
{
    NSLog(@"AccountForService call back received.");
    WrappedAccountData *data = nil;
    if(RESULT_NoErr == res)
    {
        data = [[WrappedAccountData alloc] init];
        data.account = account;
    }
        
    NSNotification *notify = [NSNotification notificationWithName:@"AccountFetched" object:data];
    [[NSNotificationCenter defaultCenter] postNotification:notify];

}

- (void) getAllAccount:(ServiceProvider) serviceID{
    AppController &applicationCtr = AppController::instance();
    
    applicationCtr.SetThumbnailSize(NewGroup::Size(mThumbnailSize.width, mThumbnailSize.height));
    
    applicationCtr.GetAllAccountForService(serviceID, CallbackAccountForService);
}

-(void)onSycTimerEvent:(NSTimer *)timer
{
    //mSyncTimer = nil;
    NSNumber* accountID = [timer userInfo];
    NSLog(@"Sync account timer event recvd for accountID: %@", accountID);
    [mSyncTimerDict removeObjectForKey:accountID];
    AppController &applicationCtr = AppController::instance();
    applicationCtr.SyncAccount([accountID integerValue], std::bind(CallBackAccountForMedia, std::placeholders::_1,
                                                    std::placeholders::_2, std::placeholders::_3, NO, self));
}
-(void)restartSyncTimer: (NSNumber *) accountID{
    if(![mSyncTimerDict objectForKey:accountID])
    {
        NSTimer *syncTimer = [NSTimer scheduledTimerWithTimeInterval:60 target:self selector:@selector(onSycTimerEvent:) userInfo:accountID repeats:NO];
        [mSyncTimerDict setObject:syncTimer forKey:accountID];
    }
}

// REVISIT: (abedi) We can consider merging various callback methods for a single notification.
void CallBackQueryForMedia(RESULT res, ODMedia media, bool deleted, bool fromCache, id callerObject, id searchView)
{
    // Update the search View
    if(res == RESULT_NoErr)
    {
        Media *mediaObj = [[Media alloc] initWithValue:media];
 
#if 0
        if([searchView isKindOfClass:[SearchViewController class]])
        {
            SearchViewController *tableViewController = (SearchViewController*)searchView;
            [tableViewController addMediaObject:mediaObj];
        }
#endif
        RootViewController *controller = (RootViewController*)searchView;
        ODMedia mObj;
        [mediaObj getODMedia:mObj];
        [controller addSearchObject:mObj];
        [controller reloadData];
    }

}

void CallBackAccountForMedia(RESULT res, ODMedia media, bool deleted, bool fromCache, id callerObject)
{
    NSLog(@"AccountForMedia call back received: From Cache: %d", fromCache);
    
    if(res == RESULT_NoErr)
    {
        WrappedMediaData *data = [[WrappedMediaData alloc] init];
        data.media = media;
        data.isDeleted = deleted;
        NSNotification *notify = [NSNotification notificationWithName:@"MediaNotification" object:data];
        [[NSNotificationCenter defaultCenter] postNotification:notify];
    }
    
    if(fromCache){
        if([callerObject isKindOfClass:[RootViewController class]])
        {
            RootViewController *viewController = (RootViewController *)callerObject;
            if ([viewController getSyncRequestPendingStatus:[NSNumber numberWithInt:media.account_id]])
            {
                [viewController setSyncRequestPendingStatus:[NSNumber numberWithInt:media.account_id] value:[NSNumber numberWithInt:0]];

                AppController &applicationCtr = AppController::instance();
                applicationCtr.SyncAccount(media.account_id, std::bind(CallBackAccountForMedia, std::placeholders::_1,
                                                                       std::placeholders::_2, std::placeholders::_3, NO, callerObject));
            }
        }
        
    }
    else{
        [callerObject performSelectorOnMainThread:@selector(restartSyncTimer:) withObject:[NSNumber numberWithInt:media.account_id] waitUntilDone:NO];
        //NSRunLoop *runner = [NSRunLoop currentRunLoop];
        //[runner addTimer:timerObject forMode: NSDefaultRunLoopMode];
    }
}

-(void) ProcessMedia:(NSNotification *) notification
{
    if(notification)
    {
        WrappedMediaData * data = [notification object];
        //NSArray *chunks = [data.media.full_path.getPlatformString() componentsSeparatedByString: @"/"];
        //NSNumber *account_id = [NSNumber numberWithLong:data.media.account_id];
        //TreeNode *node = [accountDictionary valueForKey:[NSString stringWithFormat:@"%@",account_id]];
        //TreeNode *tmp = node;
        //NSMutableString *path = [[NSMutableString alloc] initWithCapacity:1];
        //Media *media = [[Media alloc] initWithValue:data.media];
        
        auto fileNodeIndex = FileNodeIndex::GetFileNodeIndex(data.media.account_id);
        
        if (data.isDeleted) {
            if (fileNodeIndex) {
                fileNodeIndex->RemoveNode(data.media.media_id, data.media.is_folder);
            }
        }
        else {
            auto fileNode = fileNodeIndex->AddNode(data.media);
            //fileNode->UpdateMedia(data.media, selectedSortArrangement);
        }
        
        /*for(NSString *str in chunks)
        {
            if([str length]==0)
                continue;
            [path appendString:@"\\"];

            TreeNode *child = [tmp.children valueForKey:str];
            if(child == nil)
            {
                //add child
                child = [[TreeNode alloc] initWithValue:str];
                child.attribute = media;
                
                [tmp addChild:child forIdentifier:str];
                
                MediaAttribute *attrib = [[MediaAttribute alloc] initWithValue:node];
                attrib.path = path;
                
                media.attrib = attrib;
                child.inclusive = false;
            }
            else
            {
                Media* childMedia = [child getNodeMedia];
                if ([childMedia.name compare:media.name] == NSOrderedSame) {
                    child.attribute = media;
                }
            }
            
            [path appendString:str];
            tmp = child;
        }
        [treeNode flattenElementsWithCacheRefresh:YES];*/

        if (data.media.account_id == [[[CommonUIUtils getInstance] getActiveAccountID] intValue]) {
            [self performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    }
}

-(void) ProcessMediaUpdate:(NSNotification *) notification
{
    if(notification)
    {
        WrappedMediaData * data = [notification object];
        [self ProcessMedia:notification];
        NSNotification *notify = [NSNotification notificationWithName:@"MediaUpdateDeleteNotification" object:data];
        [[NSNotificationCenter defaultCenter] postNotification:notify];
    }
}

-(void) FetchAccount:(NSNotification *) notification
{
    bool bReload = false;
    if(notification)
    {
        WrappedAccountData * data = [notification object];
        
        /*TreeNode *node1 = [[TreeNode alloc] initWithValue:data.account.account_name.getPlatformString()];
        Account *acc = [[Account alloc] initWithValue:data.account];
        
        node1.attribute = acc;

        [accountDictionary setObject:node1 forKey:[NSString stringWithFormat:@"%@",acc.account_id]];*/
        
        FileNodeIndex::AddFileNodeIndex(data.account.account_id);
        
        //TODO: Move it to appropriate place when selection is changed from master
        mSelectedItem = FileNodeIndex::GetFileNodeIndex(data.account.account_id)->GetRootNode();
        
        //[treeNode addChild:node1 forIdentifier:data.account.account_name.getPlatformString()];
        
        //[treeNode flattenElementsWithCacheRefresh:YES];
        //selectedNode = treeNode;
        //selectedIndex = 0;
        //selectedNodeChildren = [selectedNode.children allKeys];

        AppController &applicationCtr = AppController::instance();
        [self setSyncRequestPendingStatus:[NSNumber numberWithInt:data.account.account_id] value:[NSNumber numberWithInt:1]];

        applicationCtr.GetMediaForAccount(data.account.account_id, std::bind(CallBackAccountForMedia, std::placeholders::_1,
                                                                             std::placeholders::_2, std::placeholders::_3, YES, self));
        
        if (data.account.account_id == [[[CommonUIUtils getInstance] getActiveAccountID] intValue]) {
            bReload = true;
        }
        
	}
    if (bReload) {
        [self performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    }
}

-(void)pushToCurrentView
{
    folderStack.push(mSelectedItem);
    if (folderStack.size() == 1) {
        self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc]
                                                 initWithTitle:@"Back" style: UIBarButtonItemStyleBordered
                                                 target:self action:@selector(popFromCurrentView)];
        
    }
}

-(void)popFromCurrentView
{
    if (!folderStack.empty()) {
        auto selectedItem = folderStack.top();
        folderStack.pop();
        
        mSelectedItem = selectedItem;
            
        self.title = selectedItem->GetName().getPlatformString();
        folderItem.title = selectedItem->GetName().getPlatformString();
        [collectionView reloadData];
    }
    
    if (folderStack.empty()) {
        self.navigationItem.leftBarButtonItem = nil;
    }
}

-(void)setAccountInView:(NSNumber*)accountID
{
    if (FileNodeIndex::GetFileNodeIndex([accountID intValue]).get()) {
        mSelectedItem = FileNodeIndex::GetFileNodeIndex([accountID intValue])->GetRootNode();
        [self performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    }
}

-(NSNumber*)getSyncRequestPendingStatus:(NSNumber*)accountID
{
    return [mSyncRequestPendingDict objectForKey:accountID];
}

-(void)setSyncRequestPendingStatus:(NSNumber*)accountID value:(NSNumber*)status
{
    [mSyncRequestPendingDict setObject:status forKey:accountID];
}

-(void) addSearchObject:(ODMedia &) mObj
{
    searchMediaObjs.push_back(mObj);
}

- (IBAction)search:(id)sender {
}

-(void) searchQuery:(UITapGestureRecognizer *)gr
{
    if(self.searchPopCntr == nil)
    {
        PopoverTableViewController *searchOption = [[PopoverTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
        searchOption.delegate = self;
        searchOption.selected = -1;
        
        [searchOption initSectionArray:5];
        PopupBase* item1 = [[PopupBase alloc] init];
        item1.index = 0;
        item1.name = @"Images";
        item1.image = @"";
        [searchOption addToArray:0 objectToAdd:item1];
        
        PopupBase* item2 = [[PopupBase alloc] init];
        item2.index = 1;
        item2.name = @"Videos";
        item2.image = @"";
        [searchOption addToArray:0 objectToAdd:item2];
        
        PopupBase* item3 = [[PopupBase alloc] init];
        item3.index = 2;
        item3.name = @"PDF";
        item3.image = @"";
        [searchOption addToArray:0 objectToAdd:item3];
        
        PopupBase* item4 = [[PopupBase alloc] init];
        item4.index = 3;
        item4.name = @"Text";
        item4.image = @"";
        [searchOption addToArray:0 objectToAdd:item4];
        
        PopupBase* item5 = [[PopupBase alloc] init];
        item5.index = 4;
        item5.name = @"Other";
        item5.image = @"";
        [searchOption addToArray:0 objectToAdd:item5];
        
        self.searchPopCntr = [[UIPopoverController alloc]initWithContentViewController:searchOption] ;
    }
    if([self.searchPopCntr isPopoverVisible])
        [self.searchPopCntr dismissPopoverAnimated:YES];
    else
        [self.searchPopCntr presentPopoverFromRect:CGRectMake(15, 30, 3, 10) inView:self.searchBar
                          permittedArrowDirections:UIPopoverArrowDirectionLeft|UIPopoverArrowDirectionUp animated:YES];
    

    
 /*   // Popup the options
    SearchQueryController *queryCntr = [[SearchQueryController alloc]
                             initWithStyle:UITableViewStylePlain];
        queryCntr.delegate = self;
        self.searchPopCntr = [[UIPopoverController alloc]
                                    initWithContentViewController:queryCntr];
    [self.searchPopCntr presentPopoverFromRect:CGRectMake(15, 30, 3, 10) inView:self.searchBar
                                    permittedArrowDirections:UIPopoverArrowDirectionLeft|UIPopoverArrowDirectionUp animated:YES];
   */ 
}

-(void) executeQuery :(NSString *)query
{
    // Execute the query
    std::string mimeType = "image/*";
    if ([query compare:@"Images"] == NSOrderedSame) {
        mimeType = "image";
    } else if ([query compare:@"Videos"] == NSOrderedSame) {
        mimeType = "video";
    } else if ([query compare:@"PDF"] == NSOrderedSame){
         mimeType = "pdf";
    } else if ([query compare:@"Documents"] == NSOrderedSame){
         mimeType = "document";
    } else if ([query compare:@"Other"] == NSOrderedSame){
         mimeType = "folder";
    }

    AppController &applicationCtr = AppController::instance();
    applicationCtr.cancelLastQuery();
    searchMediaObjs.clear();
    [self showFavoritesBar];
    [[self.mFavoritesToolBar.items objectAtIndex:1] setTitle:[NSString stringWithFormat:@"Showing All %@",query]];
 //   [self.mFavoritesToolBar setFiltersText:[NSString stringWithFormat:@"Now Showing %@",query]];
    [self reloadData];
    if ([query compare:@"Favorites"] == NSOrderedSame)
    {
        applicationCtr.GetFavoriteMedia(std::bind(CallBackQueryForMedia, std::placeholders::_1,
                                                  std::placeholders::_2, std::placeholders::_3, YES, self,self));
    }
    else
    {
   
    applicationCtr.GetMediaWithMimeType(mimeType, std::bind(CallBackQueryForMedia, std::placeholders::_1,
                                                                std::placeholders::_2, std::placeholders::_3, YES, self,self));
    }
   
    
    
}
- (void)searchBarCancelButtonClicked:(UISearchBar *) searchBar
{
   // [self.searchBar resignFirstResponder];
}
- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText
{
    // Search changed, cancel previous one and start the new one. Update the search view accordingly.
    // This is the necessary method to implement, other 2 (below) can be ignored for now.
    if(lastSearch == searchText)
        return;
    self.lastSearch = searchText;
    searchMediaObjs.clear();
    AppController &applicationCtr = AppController::instance();
    applicationCtr.cancelLastQuery();
    UString searchString = [searchText UTF8String];
    UString relativePathString = "";

    
    if(mSearchBar.mSegmentControl.selectedSegmentIndex == 1)
    {
        auto media = mSelectedItem->GetMedia();
        relativePathString = media.full_path;
    }
    [self reloadData];
    applicationCtr.GetMediaWithProperty(relativePathString, searchString, std::bind(CallBackQueryForMedia, std::placeholders::_1,
                                                                std::placeholders::_2, std::placeholders::_3, YES, self,self));
   
}

-(void) reloadQuery
{
    searchMediaObjs.clear();
    AppController &applicationCtr = AppController::instance();
    applicationCtr.cancelLastQuery();
    UString relativePathString = "";
    if(mSearchBar.mSegmentControl.selectedSegmentIndex == 1)
    {
        auto media = mSelectedItem->GetMedia();
        relativePathString = media.full_path;
    }
    [self reloadData];
    applicationCtr.GetMediaWithProperty(relativePathString, lastSearch, std::bind(CallBackQueryForMedia, std::placeholders::_1,
                                                                                    std::placeholders::_2, std::placeholders::_3, YES, self,self));
}

-(void) reloadData
{
    //[tableview reloadData];
    [collectionView reloadData];
}
#pragma mark - gesture-recognition action methods
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    CGPoint touchPoint = [touch locationInView:collectionView];
    NSIndexPath *indexPath = [collectionView indexPathForItemAtPoint:touchPoint];
    if (indexPath && [gestureRecognizer isKindOfClass:[UITapGestureRecognizer class]])
    {
        return NO;
    }
   
    touchPoint = [touch locationInView:self.searchBar];
     if([self.searchBar pointInside:touchPoint withEvent:nil])
     {
         if(touchPoint.x < 40)
         {
             [self.searchBar resignFirstResponder];
             return YES;
         }
        return NO;
     }
   
    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

- (void)activateDeletionMode:(UILongPressGestureRecognizer *)gr
{
    if (gr.state == UIGestureRecognizerStateBegan)
    {
        NSIndexPath *indexPath = [collectionView indexPathForItemAtPoint:[gr locationInView:collectionView]];
        if (indexPath)
        {
            isDeletionModeActive = YES;
            ODCollectionViewFlowLayout  *layout = (ODCollectionViewFlowLayout *)collectionView.collectionViewLayout;
            [layout invalidateLayout];
        }
    }
}

- (void)endDeletionMode:(UITapGestureRecognizer *)gr
{
    [self.searchBar resignFirstResponder];
    if (isDeletionModeActive)
    {
        NSIndexPath *indexPath = [collectionView indexPathForItemAtPoint:[gr locationInView:collectionView]];
        if (!indexPath)
        {
            isDeletionModeActive = NO;
            ODCollectionViewFlowLayout *layout = (ODCollectionViewFlowLayout *)collectionView.collectionViewLayout;
            [layout invalidateLayout];
        }
    }
}
#pragma mark - spring board layout delegate
- (BOOL) isDeletionModeActiveForCollectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout
{
    return isDeletionModeActive;
}

- (NSInteger) getRowWidth:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout
{
    if(displayMode == eListView)
    {
        if([[UIDevice currentDevice] orientation] ==  UIDeviceOrientationLandscapeLeft ||
           [[UIDevice currentDevice] orientation] ==  UIDeviceOrientationLandscapeRight)
            return 800;
        else return 550;
        
    }
    else
        return 120;// Remove these hardcodings.
}


@end
