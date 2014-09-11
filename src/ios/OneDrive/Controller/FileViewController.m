//
//  FileViewController.m
//  OneDriveUI
//
//  Created by Vikash Jindal on 08/01/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import "FileViewController.h"
#import "Media.h"
#include "AppController.h"
#include "FileInfoController.h"
#include "CommonUIUtils.h"
#include "PopupBase.h"
#import "WrappedMediaData.h"

@interface FileViewController ()

@end

@implementation FileViewController

@synthesize textToolbar;
@synthesize toolBarItems;
@synthesize selectionToolbar;
@synthesize textView;

-(id) init
{
    self = [super init];
    
	if (self) {
        self.view.autoresizesSubviews = TRUE;
        currentNode = nil;
        self.currentIndex = -1;
    }
    return self;

}
- (BOOL)shouldAutomaticallyForwardRotationMethods
{
    return YES;
}
- (BOOL)shouldAutomaticallyForwardAppearanceMethods
{
    return YES;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.jpg"]];
    
    currentNodeIndex = FileNodeIndex::GetFileNodeIndex(_currentAccount);
    selectedNode = currentNodeIndex->GetNode(_selectedNodeID);
    
    // recognizer for swipe gestures
    UISwipeGestureRecognizer *recognizer;
    
    // left and right swipe recognizers for left and right animation
    recognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleLeftSwipe:)];
    [recognizer setDirection:(UISwipeGestureRecognizerDirectionRight)];
    [self.imageView addGestureRecognizer:recognizer];
    [self.webView addGestureRecognizer:recognizer];
    [self.view addGestureRecognizer:recognizer];
    
    recognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleRightSwipe:)];
    [recognizer setDirection:(UISwipeGestureRecognizerDirectionLeft)];
    [self.imageView addGestureRecognizer:recognizer];
    [self.webView addGestureRecognizer:recognizer];
    [self.view addGestureRecognizer:recognizer];
    
    UIButton *infoButton = [UIButton buttonWithType:UIButtonTypeInfoDark];
	[infoButton addTarget:self
                        action:@selector(infoAction:)
              forControlEvents:UIControlEventTouchUpInside];
	UIBarButtonItem *infoButtonItem = [[UIBarButtonItem alloc] initWithCustomView:infoButton];
	self.navigationItem.rightBarButtonItem = infoButtonItem;
    
    self.textToolbar =  [[[NSBundle mainBundle] loadNibNamed:@"InputTextToolbar" owner:self options:nil] objectAtIndex:0];
    self.textToolbar.frame = self.navigationController.toolbar.frame;
    self.textToolbar.sender = self;
    
    self.toolBarItems = [selectionToolbar.items mutableCopy];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(ProcessMediaUpdate:) name:@"MediaUpdateDeleteNotification" object:nil];
}

-(void) ProcessMediaUpdate:(NSNotification *) notification
{
    if(notification)
    {
        WrappedMediaData * data = [notification object];
        if (data.media.account_id == [[[CommonUIUtils getInstance] getActiveAccountID] intValue]) {
            if (data.isDeleted == true && ![self moveNext]) {
                [self.navigationController popViewControllerAnimated:YES];
            }
        }
    }
}

-(BOOL)moveNext{
    int index = self.currentIndex + 1;
    while(index < selectedNode->getChildren().size()){
        auto node = selectedNode->GetChild(index);
        auto media = node->GetMedia();
        if(!media.is_folder){
            self.currentIndex = index;
            [self reload];
            return YES;
        }
        ++index;
    }
    
    return NO;
}

-(BOOL)movePrev{
    int index = self.currentIndex - 1;
    while(index >= 0){
        auto node = selectedNode->GetChild(index);
        auto media = node->GetMedia();
        if(!media.is_folder){
            self.currentIndex = index;
            [self reload];
            return YES;
        }
        --index;
    }
    
    return NO;
}

-(void)handleLeftSwipe:(UISwipeGestureRecognizer *)recognizer{
    NSLog(@"handleLeftSwipe");
    [self movePrev];
}

-(void)handleRightSwipe:(UISwipeGestureRecognizer *)recognizer{
    NSLog(@"handleRightSwipe");
    [self moveNext];
}

- (IBAction)infoAction:(id)sender
{
    FileInfoController *fileInfoController = [[FileInfoController alloc] initWithStyle:UITableViewStylePlain];
    [fileInfoController setMediaNode:currentNode->GetMedia()];
    //fileInfoController.selectedNode = currentNode;
    //for providing navigation to sub views in settings dialog

    if(!infoPopup) {
        infoPopup = [[UIPopoverController alloc] initWithContentViewController:fileInfoController];
    }
    else {
        // Setup the popover for use from the navigation bar.
        infoPopup.contentViewController = fileInfoController;
    }

	infoPopup.popoverContentSize = CGSizeMake(fileInfoController.view.frame.size.width, fileInfoController.view.frame.size.height);
    
    [infoPopup presentPopoverFromBarButtonItem:self.navigationItem.rightBarButtonItem permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
    

}

- (void)refreshView:(NSArray*)array{
    NSNumber* mediaId = [array objectAtIndex:0];
    NSString *filePath = [array objectAtIndex:1];
    
    if (self.currentIndex != -1) {
        currentNode = selectedNode->GetChild(self.currentIndex);
    }
    else {
        currentNode = selectedNode;
    }
    
    auto media = currentNode->GetMedia();
    
    
    if(media.media_id == [mediaId longLongValue]){
        UIImage *image = [UIImage imageWithContentsOfFile:filePath];
        downloadedPath = filePath;
        if(image){
            [self.activityIndicator stopAnimating];
            [self.activityIndicator setHidden:YES];
            [self.imageView setHidden:NO];
            self.imageView.image = image;
            
            AppController &applicationCtr = AppController::instance();
            
            if (applicationCtr.IsMediaFavorite(media.media_id))
                self.favoriteButton.image = [UIImage imageNamed:@"star-mini.png"];
            else
                self.favoriteButton.image = [UIImage imageNamed:@"star-empty.png"];
            applicationCtr.AddToRecent(media.media_id);
            
            NSNotification *notification = [NSNotification notificationWithName:@"UpdateRecentView" object:nil];
            [[NSNotificationCenter defaultCenter] postNotification:notification];

        }
        else{
            NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL fileURLWithPath:filePath]];
            [self.webView loadRequest:request];
        }
    }
}

- (void)viewWillAppear:(BOOL)animated{
    [self reload];
}
- (IBAction)dismissFullScreen:(id)sender {
    if(optionsPopup && [optionsPopup isPopoverVisible]){
        [optionsPopup dismissPopoverAnimated:YES];
    }
    
    downloadEventHandler = nullptr;
    [self dismissViewControllerAnimated:NO completion:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void) reload
{
    //self.webView.backgroundColor = [UIColor clearColor];
    [self.webView setHidden:YES];
    [self.imageView setHidden:YES];
    self.webView.delegate = self;
    [self.activityIndicator startAnimating];
    [self.activityIndicator setHidden:NO];
    downloadedPath = nil;
    AppController &applicationCtr = AppController::instance();
    if(!downloadEventHandler)
    {
        downloadEventHandler = applicationCtr.GetDownloadManager()->MakeShared([self](AppController::DownloadInfoData_t downloadInfo){            
            switch(downloadInfo->downloadStatus)
            {
                case DownloadInfo::kDownloadCompleted:
                {
                    [self performSelectorOnMainThread:@selector(refreshView:) withObject:@[[NSNumber numberWithLongLong:downloadInfo->mediaID], downloadInfo->localPath.getPlatformString()] waitUntilDone:NO];
                }
                    break;
                case DownloadInfo::kDownloadFailed:
                case DownloadInfo::kDownloadCancelled:
                {
                    
                }
                    break;
            }
        });
        
        applicationCtr.GetDownloadManager()->AddObserver(downloadEventHandler);
    }
    
    if (self.currentIndex != -1) {
        currentNode = selectedNode->GetChild(self.currentIndex);
    }
    else {
        currentNode = selectedNode;
    }
    
    if(currentNode && currentNode->isMediaValid()){
        auto media = currentNode->GetMedia();
        //self.fileNameLabel.text = media.name;
        self.navigationItem.title = currentNode->GetName().getPlatformString();
        applicationCtr.DownloadFile(media.account_id, media.media_id);
    }

}

- (void)webViewDidFinishLoad:(UIWebView *)webView{
    [self.activityIndicator stopAnimating];
    [self.activityIndicator setHidden:YES];
    [self.webView setHidden:NO];
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error{
    
}

- (void)moveItem:(id)sender {
}


- (void) reportShareError:(NSString *)error{
    [[[UIAlertView alloc] initWithTitle:@"Share Error"
                                message:error
                               delegate:nil
                      cancelButtonTitle:@"OK"
                      otherButtonTitles:nil]
     show];
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex{
    if(buttonIndex == 1){
        [self removeItem:nil];
    }
}

-(void) removeItem:(id)sender{
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    [pUtils initialize:self];
    
    if(currentNode->isMediaValid()){
        auto media = currentNode->GetMedia();
        AppController &applicationCtr = AppController::instance();
        
        CommonUIUtils *pUtils = [CommonUIUtils getInstance];
        [pUtils initialize:self];
        [pUtils showProgress: @"Deleting file"];
        
        applicationCtr.DeleteFile(media.account_id, media.media_id, [self, pUtils](bool result, ODMedia media){
            [pUtils hideProgress];
            if(result){
                [pUtils mediaDeleted:media];
            }
            else{
                [pUtils showError:@"Deletion"];
            }
            
        });
    }
    else{
        
    }
}

-(void) shareItem:(id)sender{

    if(currentNode->isMediaValid()){
        auto media = currentNode->GetMedia();
            AppController &applicationCtr = AppController::instance();
            applicationCtr.ShareFileWithComments(kShareViaFacebook, media.media_id, media.account_id, UString("Test Message"), [self, media](ShareServiceResult result, const UString &errorString){
                if(result == kNoError){
                    
                }
                else{
                    [self performSelectorOnMainThread:@selector(reportShareError:) withObject:errorString.getPlatformString() waitUntilDone:NO];
                }
                
            });
        }
        else{
            
        }
}

void CallbackForCopyMove(bool success, ODMedia media)
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
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    [pUtils initialize:self];
    
    [pUtils showProgress: workflow == 0 ? @"Copying" : @"Moving"];
    
    if(currentNode->isMediaValid()){
        auto media = currentNode->GetMedia();
        
        /*UString fileName;
        UString filePath;
        Utils::SplitFilePathAndName(media.file_id, filePath, fileName);
        
        NSString* path = parent.full_path.getPlatformString();
        path = [path stringByAppendingString:@"/"];
        path = [path stringByAppendingString:fileName.getPlatformString()];*/
        
        AppController &applicationCtr = AppController::instance();
        
        if (workflow == 0) { //Copy
            applicationCtr.CopyFile(media.account_id, media.media_id, parent.file_id, CallbackForCopyMove);
        }
        else { //Move
            applicationCtr.MoveFile(media.account_id, media.media_id, parent.file_id, CallbackForCopyMove);
        }
    }
}

- (void) takenInputAtIndex:(UITableViewController *)popover selectedIndex:(NSInteger) index{
    switch(index){
        case 2:
        case 3:
        {
            //Cut/Copy
            ListViewController* listCntrl = [[ListViewController alloc] init];
            listCntrl.delegate = self;
            
            ODMedia media;
            media.media_id = -1;
            [listCntrl setParentMedia:media forWorkflow:index == 2 ? 0 : 1];
            
            [popover.navigationController pushViewController:listCntrl animated:YES];

            /*CommonUIUtils *pUtils = [CommonUIUtils getInstance];
            NSMutableArray *strArray = [[NSMutableArray alloc] init];
            
            auto media = currentNode->GetMedia();
            [strArray addObject:currentNode->GetName().getPlatformString()];
            
            UIPasteboard *clipboard = [pUtils createClipboard:index == 1 ? kClipBoardForCopy : kClipBoardForCut];
            clipboard.strings = strArray;*/
        }
            break;
        case 0:
        {
            //Facebook
            [optionsPopup dismissPopoverAnimated:YES];
            [self shareItem:nil];
        }
            break;
        case 4:
        {
            //Open In
            [optionsPopup dismissPopoverAnimated:YES];
            if(downloadedPath){
                self.docInteractionController = [UIDocumentInteractionController interactionControllerWithURL:[[NSURL alloc] initFileURLWithPath:downloadedPath]];
                BOOL displayed = [self.docInteractionController presentOptionsMenuFromBarButtonItem:self.actionButton animated:YES];
                if(!displayed){
                    [[[UIAlertView alloc] initWithTitle:@"Open Error"
                                                message:@"No application installed which can open this type of file."
                                               delegate:nil
                                      cancelButtonTitle:@"OK"
                                      otherButtonTitles:nil]
                     show];
                }
            }
            
        }
            break;
    }
    
    
}

- (IBAction)showOptions:(id)sender {
    if(optionsPopup && [optionsPopup isPopoverVisible]){
        [optionsPopup dismissPopoverAnimated:YES];
        return;
    }
    
    PopoverTableViewController *shareOptions = [[PopoverTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
    shareOptions.delegate = self;
    shareOptions.selected = -1;
    
    [shareOptions initSectionArray:4];
    
    PopupBase* item1 = [[PopupBase alloc] init];
    item1.index = 0;
    item1.name = @"Facebook";
    item1.image = @"";
    [shareOptions addToArray:1 objectToAdd:item1];

    PopupBase* item2 = [[PopupBase alloc] init];
    item2.index = 1;
    item2.name = @"Twitter";
    item2.image = @"";
    [shareOptions addToArray:1 objectToAdd:item2];
    
    PopupBase* item3 = [[PopupBase alloc] init];
    item3.index = 2;
    item3.name = @"Copy";
    item3.image = @"";
    [shareOptions addToArray:2 objectToAdd:item3];
    
    PopupBase* item4 = [[PopupBase alloc] init];
    item4.index = 3;
    item4.name = @"Move";
    item4.image = @"";
    [shareOptions addToArray:2 objectToAdd:item4];

    PopupBase* item5 = [[PopupBase alloc] init];
    item5.index = 4;
    item5.name = @"Open In...";
    item5.image = @"";
    [shareOptions addToArray:3 objectToAdd:item5];
    
    //for providing navigation to sub views in this dialog
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:shareOptions];
    navigationController.delegate = shareOptions;
    
	if(!optionsPopup) {
        optionsPopup = [[UIPopoverController alloc] initWithContentViewController:navigationController];
    }
    else {
        // Setup the popover for use from the navigation bar.
        optionsPopup.contentViewController = navigationController;
    }
    
    [optionsPopup presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];

}

- (IBAction)markFavorite:(id)sender {
    if(currentNode->isMediaValid()){
        auto media = currentNode->GetMedia();
        AppController &applicationCtr = AppController::instance();
        
        if (applicationCtr.IsMediaFavorite(media.media_id)) {
            applicationCtr.RemoveMediaFromFavorite(media.media_id);
            self.favoriteButton.image = [UIImage imageNamed:@"star-empty.png"];
        }
        else {
            applicationCtr.SetMediaAsFavorite(media.media_id);
            self.favoriteButton.image = [UIImage imageNamed:@"star-mini.png"];
        }
    }
    else {
    }
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
    auto media = currentNode->GetMedia();
    
    if(media.file_name.getPlatformString() == editedName)
        return; // Same name, nothing to do.
    
    CommonUIUtils *pUtils = [CommonUIUtils getInstance];
    [pUtils initialize:self];
    
    [pUtils showProgress:@"Renaming"];

    AppController &applicationCtr = AppController::instance();
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

- (IBAction)renameFile:(id)sender {
    auto media = currentNode->GetMedia();
    
    
    self.textView = [[TextView alloc ]initWithText:media.file_name.getPlatformString() withFrame:self.view.frame] ;
    [selectionToolbar setItems:[NSMutableArray arrayWithArray:self.textToolbar.items] animated:YES];
    
    [self.view addSubview:textView.view];
    [self.view bringSubviewToFront:textView.view];

   // [self moveItem:nil];
}

- (IBAction)deleteItem:(id)sender {
    //Delete
    [[[UIAlertView alloc] initWithTitle:@"Deleting file"
                                message:[NSString stringWithFormat:@"%@ will be deleted. Are you sure?", currentNode->GetName().getPlatformString()]
                               delegate:self
                      cancelButtonTitle:@"No"
                      otherButtonTitles:@"Yes", nil]
     show];
}
@end
