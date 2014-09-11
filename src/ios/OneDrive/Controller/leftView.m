//
//  leftView.m
//  OneDrive_temp
//
//  Created by Ajay on 4/14/13.
//  Copyright (c) 2013 Ajay. All rights reserved.
//

#import "leftView.h"
#import "AccountViewController.h"
#import "SplitViewController.h"
#import "AppController.h"
#import "FileViewController.h"
#import "SearchViewController.h"
#import "CommonUIUtils.h"

@interface leftView ()

@end

@implementation leftView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self)
    {
        // Add the inner View]
    }
    return self;
}

@end


@implementation leftViewController

@synthesize sectionsData;
@synthesize _controller;
@synthesize splitViewController;

-(id)initWithSplitViewController:(SplitViewController*)splitViewCtrl
{
    self = [super init];
    if(self){
        splitViewController = splitViewCtrl;
    }
    
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    _controller = [[leftTreeViewController alloc] initWithStyle:UITableViewStyleGrouped];
    [_controller setSource:self];
    [_controller setItems: self.sectionsData];
    [_controller.view setFrame:CGRectMake(0, 0, 200, self.view.frame.size.height)];
    [self.view setAutoresizesSubviews:YES];
     self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.jpg"]];
    [self.view addSubview:_controller.view];
}

- (void) setTableData : (NSArray *) data
{
    self.sectionsData = data;
}

- (void) callBackForLeftViewController:(NSIndexPath*)indexPath forObjectId:(NSNumber*)objectId
{
    NSInteger index = indexPath.section;
    NSInteger row = indexPath.row;
    if (index == 0) {
        
        if (row == 0) {
            AccountViewController* accountCntr = [[AccountViewController alloc] initWithStyle:UITableViewStyleGrouped];
            UINavigationController* navCntr = [[UINavigationController alloc] initWithRootViewController:accountCntr];
            [splitViewController setDetailController:navCntr];
        }
        else {
            [[CommonUIUtils getInstance] setActiveAccountID:objectId];
            [splitViewController showRootController];
        }
    }
    else if (index ==1){
        NSString* query;
        switch (row) {
            case 0:
                query = @"Images";
                break;
            case 1:
                query = @"Videos";
                break;
            case 2:
                query = @"Documents";
                break;
            case 3:
                query = @"PDF";
                break;
            case 4:
                query = @"Favorites";
                break;
            default:
                break;
        }
       // [splitViewController showRootController];
        [splitViewController setSearchMode:query];
    }
    else if (index == 2){
        AppController &applicationCtr = AppController::instance();
        ODMediaList recentList;
        applicationCtr.GetRecents(recentList);
        // Find media id from the recent's list
        int index = 0;
        for (int i = 0 ; i < recentList.size() ; i++)
        {
            if(recentList[i].media_id == objectId.integerValue)
                index = i;
        }
        ODMedia media = recentList.at(index);
        
        FileViewController* fileViewController = [self.splitViewController.storyboard instantiateViewControllerWithIdentifier:@"fullScreenView"];
        fileViewController.selectedNodeID = media.media_id;
        fileViewController.currentAccount = [[[CommonUIUtils getInstance] getActiveAccountID] intValue];
        fileViewController.currentIndex = -1;
        [splitViewController setDetailController:fileViewController];
    }
}

-(void) reloadLeftTree
{
    [_controller.tableView reloadData];
}

@end