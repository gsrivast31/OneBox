//
//  ListViewController.m
//  OneDrive
//
//  Created by Gaurav Srivastava on 5/29/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import "ListViewController.h"
#import "AppController.h"
#import "CommonUIUtils.h"

@interface ListViewController ()

@end

@implementation ListViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    ((UITableView*)(self.view)).rowHeight = 40;

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)performOperation
{
    if (self.delegate) {
        [self.delegate callBackFromListViewController:mParentFolder forWorkflow:mWorkflow];
    }
}

#pragma mark - Table view data source

- (void) setParentMedia:(ODMedia)media forWorkflow:(NSInteger)workflow
{
    self->mParentFolder = media;
    self->mWorkflow = workflow;
    
    ODMediaList mediaList;
    AppController &applicationCtr = AppController::instance();
    
    //A small hack to show accounts in initial view. Accounts treated as media
    if (media.media_id == -2) {
        ODAccountList accountList;
        applicationCtr.GetAllAccounts(accountList);
        
        for (ODAccountList::iterator iter = accountList.begin(); iter != accountList.end(); iter++) {
            ODMedia media;
            media.media_id = -1;
            media.account_id = (*iter).account_id;
            media.file_name = (*iter).account_name;
            
            NSString *iconPath = [[CommonUIUtils getInstance] getServiceIcon:(*iter).account_service_id];
            
            media.icon = iconPath;
            mediaList.push_back(media);
        }
    }
    else
    {
        self.title = media.file_name.getPlatformString();
        
        self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc]
                                                            initWithTitle:mWorkflow == 0 ? @"Copy" : @"Move"
                                                            style: UIBarButtonItemStyleBordered
                                                            target:self action:@selector(performOperation)];
        

        applicationCtr.GetChildMedia(media.media_id, media.account_id, mediaList);
    }

    self->mFolderList = mediaList;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return mFolderList.size();
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"FolderListCell";
    
    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }

    cell.textLabel.text = mFolderList[indexPath.row].file_name.getPlatformString();
    cell.textLabel.font = [UIFont systemFontOfSize:15.0f];
    cell.imageView.image = [UIImage imageNamed:mFolderList[indexPath.row].icon.getPlatformString()];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView indentationLevelForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 1;
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    ODMedia media = mFolderList[indexPath.row];
    
    ListViewController* listController = [[ListViewController alloc] init];
    listController.delegate = self.delegate;
    [listController setParentMedia:media forWorkflow:mWorkflow];
    [self.navigationController pushViewController:listController animated:YES];

}

@end
