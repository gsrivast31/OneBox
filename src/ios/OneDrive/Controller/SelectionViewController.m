//
//  SelectionViewController.m
//  OneDriveUI
//
//  Created by Ruchin Kansal on 01/03/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import "SelectionViewController.h"
#import "IconCell.h"
#import "ODCollectionViewFlowLayout.h"
#import "PopoverTableViewController.h"
#import "AppController.h"
#import "CommonUIUtils.h"
#import "SearchIconCell.h"
#import "PopupBase.h"
#import "ListViewController.h"
#import "WrappedMediaData.h"

@implementation SelectionViewController
@synthesize textToolbar;
@synthesize toolBarItems;
@synthesize selectionToolbar;
@synthesize textView;

- (id)initWithFrame:(CGRect)frame 
{
    self = [super init];
    //  UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    ODCollectionViewFlowLayout  *layout = [[ODCollectionViewFlowLayout alloc] init];
    UICollectionView *collectionView = [[UICollectionView alloc] initWithFrame:frame collectionViewLayout:layout];
    self.collectionView = collectionView;
    self.collectionView.frame = frame;
    self.collectionView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.jpg"]];
    self.collectionView.autoresizesSubviews= YES;
    return self;
    
}

-(void)initLayout:(BOOL)searchMode
{
//    self.mSearchMode = searchMode;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
    //auto frame = [self.view frame];
    //auto frame = [self.view frame];
    
          ODCollectionViewFlowLayout  *layout = [[ODCollectionViewFlowLayout alloc] init];
        
    self.collectionView.collectionViewLayout = layout;
    self.collectionView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.jpg"]];
    //mediaObjs = [[NSMutableArray alloc] init];
    UINib *cellNib = nil;
    cellNib = [UINib nibWithNibName:@"IconCell" bundle:nil];
    [self.collectionView registerNib:cellNib forCellWithReuseIdentifier:@"ICON"];
    
    self.collectionView.delegate = self;
    self.collectionView.dataSource = self;
    
    selectionSet = [NSMutableSet setWithCapacity:10];
    self.textToolbar =  [[[NSBundle mainBundle] loadNibNamed:@"InputTextToolbar" owner:self options:nil] objectAtIndex:0];
    self.textToolbar.frame = self.navigationController.toolbar.frame;
    self.textToolbar.sender = self;
    
    self.toolBarItems = [selectionToolbar.items mutableCopy];
    [self enableDisableToolBarItems];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(ProcessMediaUpdate:) name:@"MediaUpdateDeleteNotification" object:nil];
}

-(void) ProcessMediaUpdate:(NSNotification *) notification
{
    if(notification)
    {
        WrappedMediaData * data = [notification object];
        if (data.media.account_id == [[[CommonUIUtils getInstance] getActiveAccountID] intValue]) {
            [selectionSet removeAllObjects];
            [self.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    }
}

-(void) updateSelectionNode:(std::vector<ODMedia> &) mediaList
{
    mQueryNode.reset(new FileNode(ODMedia(),FileNode::SharedPtr()));
    FileNode::AddFileNodes(searchChildList, mediaList);
    FileNode::UpdatFileNodeWithCustomChildren(mQueryNode, searchChildList);
}

-(void) setSelectedNode: (FileNode::SharedPtr) selectedNode
{
    mQueryNode = selectedNode;
}

- (void) cleanUp
{
    [self.collectionView reloadData];
}

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return mQueryNode->getChildren().size();
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView1 cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    
    auto selectedItem = mQueryNode;
    
    UICollectionViewCell *icon ;
    icon = [collectionView1 dequeueReusableCellWithReuseIdentifier:@"ICON" forIndexPath:indexPath];
    auto node = selectedItem->GetChild(indexPath.row);
    auto media = node->GetMedia();
    
    IconCell *iCell = (IconCell*)(icon);
    iCell.label.text = node->GetName().getPlatformString();
    iCell.label.textColor = [UIColor whiteColor];
    UIImage* image = [UIImage imageNamed:media.icon.getPlatformString()];
    if (image == nil) {
      image = [UIImage imageWithContentsOfFile:media.icon.getPlatformString()];
    }
    
    iCell.thumbnail.image = image ;
    
    if([selectionSet containsObject:indexPath]){
            //Draw additional selection feedback
      UIImage *image = iCell.thumbnail.image;
            
     // begin a graphics context of sufficient size
     // FIXME: Bad harcoding... Fix it once we figure out the final size of the image thumbnail for search view.
     CGSize imageSize = image.size;
     UIGraphicsBeginImageContext(imageSize);
    // draw original image into the context
    //[image drawInRect:icon.thumbnail.bounds];
    [image drawInRect:CGRectMake(0,0,imageSize.width,imageSize.height)];
            
    UIImage *badge = [UIImage imageNamed:@"emblem-default"];
    [badge drawAtPoint:CGPointMake(imageSize.width - badge.size.width, imageSize.height - badge.size.height)];
            
    // make image out of bitmap context
    iCell.thumbnail.image = UIGraphicsGetImageFromCurrentImageContext();
            
    // free the context
    UIGraphicsEndImageContext();
        
    }
     return icon;
   
}

- (IBAction)backButtonHandler:(id)sender {
    if(sharePopover && [sharePopover isPopoverVisible]){
        [sharePopover dismissPopoverAnimated:YES];
    }
    
    [self dismissViewControllerAnimated:NO completion:nil];
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    NSLog(@"Selected!");
    if([selectionSet containsObject:indexPath]){
        [selectionSet removeObject:indexPath];
    }
    else{
        [selectionSet addObject:indexPath];
    }
    [self enableDisableToolBarItems];
    [self.collectionView reloadItemsAtIndexPaths:[NSArray arrayWithObject:indexPath]];
}

- (void) dismissEditView
{
    [textView.view removeFromSuperview];
    [selectionToolbar setItems:[NSMutableArray arrayWithArray:self.toolBarItems] animated:YES];
}

- (void) updateItemName: (NSString*) newName
{
    //newName is not used here. Will clean that up.
    NSString *editedName = textView.textField.text;
    
    [textView.view removeFromSuperview];
    [selectionToolbar setItems:[NSMutableArray arrayWithArray:self.toolBarItems] animated:YES];
    if([editedName isEqual: @""])
        return;
    
    
    // Call the rename functionality.
    NSIndexPath *indexPath = nil;
    NSEnumerator *enumerator = [selectionSet objectEnumerator];
    indexPath = [enumerator nextObject];
    auto node = mQueryNode->GetChild(indexPath.row);
    auto media = node->GetMedia();
    
    if(media.file_name.getPlatformString() == editedName)
        return; // Same name, nothing to do.
    
    AppController &applicationCtr = AppController::instance();
    std::function<void (bool success)> dummyCallback; // Handle for displaying message??? Might crash now.
    // We might need to have some global queue which displays these messages to user. Cannot handle in this class because
    // this might have got destroyed by the time rename is done.
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    [pUtils initialize:self];
    
    [pUtils showProgress:@"Renaming"];

    applicationCtr.RenameFileOrFolder(media.account_id, media.media_id, UString::fromPlatform(editedName),
                                      [pUtils](bool success, ODMedia media) {
                                          [pUtils hideProgress];
                                          if (success) {
                                              [pUtils mediaUpdated:media];
                                          }
                                          else {
                                              [pUtils showError:@"Rename"];
                                          }

                                      });
    
}

- (void) enableDisableToolBarItems {
    if ([selectionSet count] == 1)
        [self.renameButton setEnabled:TRUE];
    else
        [self.renameButton setEnabled:FALSE];
    
    [self.favoriteButton setEnabled: [selectionSet count] > 0 ? TRUE : FALSE];
}

- (IBAction)renameSelectedFiles:(id)sender {
}

- (IBAction)deleteSelectedItems:(id)sender {
    [self deleteItems:[selectionSet objectEnumerator]];
}

- (IBAction) rename:(id)sender
{
    if([selectionSet count] != 1)
        return;  //Should not have come here
  
    NSIndexPath *indexPath = nil;
    NSEnumerator *enumerator = [selectionSet objectEnumerator];
    indexPath = [enumerator nextObject];
    auto node = mQueryNode->GetChild(indexPath.row);
    auto media = node->GetMedia();

    
    self.textView = [[TextView alloc ]initWithText:media.file_name.getPlatformString() withFrame:self.collectionView.frame] ;
    [selectionToolbar setItems:[NSMutableArray arrayWithArray:self.textToolbar.items] animated:YES];

    [self.collectionView addSubview:textView.view];
    [self.collectionView bringSubviewToFront:textView.view];
}

- (IBAction) markFavoriteSelectedFiles:(id)sender {
    [self markFavorite:[selectionSet objectEnumerator]];
}
- (IBAction)shareSelectedFiles:(id)sender {
    if(sharePopover && [sharePopover isPopoverVisible]){
        [sharePopover dismissPopoverAnimated:YES];
        return;
    }
    
    PopoverTableViewController* shareOptions = [[PopoverTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
    shareOptions.delegate = self;
    shareOptions.selected = -1;
    
    [shareOptions initSectionArray:2];
    
    PopupBase* item1 = [[PopupBase alloc] init];
    item1.index = 0;
    
    if ([selectionSet count] == mQueryNode->getChildren().size())
        item1.name = @"De-Select All";
    else
        item1.name = @"Select All";
    
    item1.image = @"";
    [shareOptions addToArray:0 objectToAdd:item1];
    
    PopupBase* item2 = [[PopupBase alloc] init];
    item2.index = 1;
    item2.name = @"Copy";
    item2.image = @"";
    [shareOptions addToArray:0 objectToAdd:item2];
    
    PopupBase* item3 = [[PopupBase alloc] init];
    item3.index = 2;
    item3.name = @"Move";
    item3.image = @"";
    [shareOptions addToArray:0 objectToAdd:item3];
    
    PopupBase* item4 = [[PopupBase alloc] init];
    item4.index = 3;
    item4.name = @"Facebook";
    item4.image = @"";
    [shareOptions addToArray:1 objectToAdd:item4];
    
    PopupBase* item5 = [[PopupBase alloc] init];
    item5.index = 4;
    item5.name = @"Twitter";
    item5.image = @"";
    [shareOptions addToArray:1 objectToAdd:item5];
    
    //for providing navigation to sub views in this dialog
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:shareOptions];
    navigationController.delegate = shareOptions;
    
	if(!sharePopover) {
        sharePopover = [[UIPopoverController alloc] initWithContentViewController:navigationController];
    }
    else {
        // Setup the popover for use from the navigation bar.
        sharePopover.contentViewController = navigationController;
    }
 
    [sharePopover presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

- (void) reportShareError:(NSString *)error{
    [[[UIAlertView alloc] initWithTitle:@"Share Error"
                                message:error
                               delegate:nil
                      cancelButtonTitle:@"OK"
                      otherButtonTitles:nil]
     show];
}

void CallbackForUpload(bool success, ODMedia media)
{
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    [pUtils hideProgress];
    
    if (success) {
        [pUtils mediaUpdated:media];
    }
    else
        [pUtils showError:@"Copy"];
    
}

-(void) callBackFromListViewController:(ODMedia)parent forWorkflow:(NSInteger)workflow
{
    if(sharePopover && [sharePopover isPopoverVisible]){
        [sharePopover dismissPopoverAnimated:YES];
    }
    
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    [pUtils initialize:self];
    
    [pUtils showProgress: workflow == 0 ? @"Copying" : @"Moving"];
    
    for(NSIndexPath *indexPath in selectionSet){
        auto node = mQueryNode->GetChild(indexPath.row);
        auto media = node->GetMedia();
        
        /*UString fileName;
        UString filePath;
        Utils::SplitFilePathAndName(media.file_id, filePath, fileName);
        
        NSString* path = parent.full_path.getPlatformString();
        path = [path stringByAppendingString:@"/"];
        path = [path stringByAppendingString:fileName.getPlatformString()];*/
        
        AppController &applicationCtr = AppController::instance();
        
        if (workflow == 0) { //Copy
            applicationCtr.CopyFile(media.account_id, media.media_id, parent.file_id, CallbackForUpload);
        }
        else { //Move
            applicationCtr.MoveFile(media.account_id, media.media_id, parent.file_id, CallbackForUpload);
        }
    }
}

-(void) markFavorite:(NSEnumerator *)enumerator{
    NSIndexPath *indexPath = nil;
    while(indexPath = [enumerator nextObject]){
        auto node = mQueryNode->GetChild(indexPath.row);
        auto media = node->GetMedia();
        
        AppController &applicationCtr = AppController::instance();
        applicationCtr.SetMediaAsFavorite(media.media_id);
        
    }
    
}

-(void) deleteItems:(NSEnumerator *)enumerator{
    
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    [pUtils initialize:self];
    
    [pUtils showProgress:@"Deleting"];

    NSIndexPath *indexPath = nil;
    while(indexPath = [enumerator nextObject]){
        auto node = mQueryNode->GetChild(indexPath.row);
        auto media = node->GetMedia();
            AppController &applicationCtr = AppController::instance();
            applicationCtr.DeleteFile(media.account_id, media.media_id, [self, enumerator, pUtils](bool result, ODMedia media)
                {
                    [pUtils hideProgress];
                    if (result) {
                        [pUtils mediaDeleted:media];
                        //[self performSelectorOnMainThread:@selector(deleteItems:) withObject:enumerator waitUntilDone:NO];
                    }
                    else {
                        [pUtils showError:@"Deletion"];
                    }
                });
    }

}

-(void) shareItems:(NSArray *)array {
    NSEnumerator *enumerator = [array objectAtIndex:0];
    NSNumber *serviceKind = [array objectAtIndex:1];
    auto service = (ShareServicesType)[serviceKind intValue];
    NSIndexPath *indexPath = nil;
    while(indexPath = [enumerator nextObject]){
        auto node = mQueryNode->GetChild(indexPath.row);
        auto media = node->GetMedia();
            AppController &applicationCtr = AppController::instance();
            applicationCtr.ShareFileWithComments(service, media.media_id, media.account_id, UString("Test Message"), [self, array](ShareServiceResult result, const UString &errorString){
                if(result == kNoError){
                    [self performSelectorOnMainThread:@selector(shareItems:) withObject:array waitUntilDone:NO];
                }
                else{
                    [self performSelectorOnMainThread:@selector(reportShareError:) withObject:errorString.getPlatformString() waitUntilDone:NO];
                }
                
            });
        
    }
}

- (void) takenInputAtIndex:(UITableViewController *)popover selectedIndex:(NSInteger) index{
    switch(index){
        case 0:
        {
            if ([selectionSet count] == mQueryNode->getChildren().size())
            {
                //DeSelect All
                [selectionSet removeAllObjects];
                [self.collectionView reloadData];
            }
            else
            {
                //Select All
                for(int row = 0; row < mQueryNode->getChildren().size(); ++row){
                    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:row inSection:0];
                    if(![selectionSet containsObject:indexPath]){
                        [selectionSet addObject:indexPath];
                        [self.collectionView reloadItemsAtIndexPaths:[NSArray arrayWithObject:indexPath]];
                    }
                    
                }
            }
            [self enableDisableToolBarItems];
            [sharePopover dismissPopoverAnimated:YES];
        }
            break;
        case 1:
        case 2:
        {
            ListViewController* listCntrl = [[ListViewController alloc] init];
            listCntrl.delegate = self;
            
            ODMedia media;
            media.media_id = -2;
            [listCntrl setParentMedia:media forWorkflow:index == 1 ? 0 : 1];
            
            [popover.navigationController pushViewController:listCntrl animated:YES];
            //Cut/Copy
           /* CommonUIUtils *pUtils = [CommonUIUtils getInstance];
            NSMutableArray *strArray = [[NSMutableArray alloc] init];
            
            for(NSIndexPath *indexPath in selectionSet){
                auto node = mQueryNode->GetChild(indexPath.row);
                auto media = node->GetMedia();
                
                [strArray addObject:media.file_id.getPlatformString()];
            }
            
            UIPasteboard *clipboard = [pUtils createClipboard:index == 1 ? kClipBoardForCopy : kClipBoardForCut];
            clipboard.strings = strArray;*/
        }
            break;
            
        case 3:
        case 4:
        {
            //Facebook/Twitter
            NSMutableArray *array = [NSMutableArray arrayWithObjects:[selectionSet objectEnumerator], [NSNumber numberWithInt:(index == 3 ? kShareViaFacebook : kShareViaTwitter)], nil];
            [self shareItems:array];
            [sharePopover dismissPopoverAnimated:YES];
        }
            break;
    }
    
    //[sharePopover dismissPopoverAnimated:YES];
}
@end



@implementation InputTextToolBar
@synthesize sender;
@synthesize editedName;

-(IBAction) doneClicked:(id)view
{
    [sender updateItemName:@""];
}

-(IBAction) cancelClicked:(id)view
{
    [sender dismissEditView];
    
}

@end


@implementation TextView
@synthesize inputText;
@synthesize textField;

-(void) viewDidLoad
{
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    return YES;
}

-(id) initWithText:(NSString*) originalText withFrame:(CGRect)frameRect
{
    self = [super init];
    inputText = originalText;
    
    textField = [[UITextField alloc] init];
    [textField setAutoresizingMask:( UIViewAutoresizingFlexibleWidth |
                                    UIViewAutoresizingFlexibleHeight )];
    
   [ self.view setAutoresizingMask:( UIViewAutoresizingFlexibleWidth |
                               UIViewAutoresizingFlexibleHeight )];
    CGRect textFrame = frameRect;
    textFrame.size.height = 60;
    textFrame.origin.y += 50;
    textFrame.size.width -= 5;
    
    textField.frame= textFrame;
    textField.delegate = self;
    [textField setPlaceholder:originalText];
    [textField becomeFirstResponder];
    textField.textColor = [UIColor blackColor];
    textField.font = [UIFont italicSystemFontOfSize:30.0f];
    textField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    textField.textAlignment = NSTextAlignmentCenter;
    
    //Border related changes.
    textField.borderStyle=UITextBorderStyleNone;
    textField.layer.cornerRadius=8.0f;
    textField.layer.masksToBounds=YES;
    textField.backgroundColor=[UIColor redColor];
    textField.layer.borderColor=[[UIColor blackColor]CGColor];
    textField.layer.borderWidth= 1.0f;
    textField.clearButtonMode = UITextFieldViewModeAlways;
    
    textField.backgroundColor = [UIColor whiteColor];
    
    [self.view addSubview:textField];

    
  //  self.opaque = YES;
    
    return self;
}

@end
