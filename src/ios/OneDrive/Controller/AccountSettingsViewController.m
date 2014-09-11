//
//  AccountSettingsViewController.m
//  OneDrive
//
//  Created by Gaurav Srivastava on 6/17/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import "AccountSettingsViewController.h"
#import "AccountSettingsCell.h"
#import "CommonUIUtils.h"

@interface AccountSettingsViewController ()

@end

static NSInteger const kCellHeight  = 40;
static NSInteger const kNavigationbarHeight  = 30;
static NSInteger const kSectionHeaderHeight  = 4;
static NSInteger const kSectionFooterHeight  = 2;

@implementation AccountSettingsViewController

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

    self.title = NSLocalizedString(@"Account Sync Settings", @"nav bar title");

    //((UITableView*)(self.view)).rowHeight = kCellHeight;
    //((UITableView*)(self.view)).sectionHeaderHeight = kSectionHeaderHeight;
    //((UITableView*)(self.view)).sectionFooterHeight = kSectionFooterHeight;

    AppController &applicationCtr = AppController::instance();
    applicationCtr.GetAllAccounts(accountList);
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

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return accountList.size();
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 1;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"AccountSettingsCell";
    
    AccountSettingsCell* cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[AccountSettingsCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    cell.nameLabel.text = accountList[indexPath.row].account_name.getPlatformString();
    cell.storageLabel.text = accountList[indexPath.row].account_name.getPlatformString();
    [cell.status setOn:accountList[indexPath.row].account_status animated:NO];
    
    NSString* iconName = [[CommonUIUtils getInstance] getServiceIcon:accountList[indexPath.row].account_service_id];
    cell.imageView.image = [UIImage imageNamed:iconName];
    
    return cell;

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
    // Navigation logic may go here. Create and push another view controller.
    /*
     <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
     [self.navigationController pushViewController:detailViewController animated:YES];
     */
}

@end
