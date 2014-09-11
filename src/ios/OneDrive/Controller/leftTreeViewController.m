//
//  leftTreeViewController.m
//  OneDrive_temp
//
//  Created by Ajay on 4/14/13.
//  Copyright (c) 2013 Ajay. All rights reserved.
//

#import "leftTreeViewController.h"
#import "AppController.h"
#import "Media.h"
#import "CommonUIUtils.h"

static NSInteger const kCellHeight  = 35;
static NSInteger const kHeaderHeight = 60;
static NSInteger const kFooterHeight = 30;
NSString * const kObjectsString = @"Objects";
NSString * const kObjectIdsString = @"ObjectIds";
NSString * const kTitleString = @"Title";    // Key for the section's dictionary


@interface leftTreeViewController ()

@end

@implementation leftTreeViewController

@synthesize listOfItems;
@synthesize mSource;
@synthesize mtableView;


- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(void) setItems : (NSArray *) items
{
    self.listOfItems = items;
}

-(void) updateRecentView:(NSNotification*)notification
{
    NSMutableDictionary *dictionary = [listOfItems objectAtIndex:2];
    
    AppController &applicationCtr = AppController::instance();
    ODMediaList recentList;
    applicationCtr.GetRecents(recentList);

    NSMutableArray *recents = [[NSMutableArray alloc] init];
    int selectedRow = 0;
    for (int i=0; i<recentList.size(); i++) {
        ODMedia media = recentList[i];
        if(media.media_id == recentMediaIDSelected)
            selectedRow = i;
        [recents addObject:[[Media alloc ]initWithValue:media]];
    }
    
    [dictionary setObject:recents forKey:kObjectsString];

  //  NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:2];
    
    [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:2] withRowAnimation:UITableViewRowAnimationAutomatic];
    
    //Preserve previous selection. We have to do it, we are reloading the data here.
    [self.tableView selectRowAtIndexPath:[NSIndexPath indexPathForRow:selectedRow inSection:2] animated:YES scrollPosition:UITableViewScrollPositionNone];
}

-(void) updateAccountView:(NSNotification*)notification
{
    NSMutableDictionary *dictionary = [listOfItems objectAtIndex:0];
    
    AppController &applicationCtr = AppController::instance();
    ODAccountList acctList;
    applicationCtr.GetAllAccounts(acctList);
    
    NSMutableArray *accounts = [[NSMutableArray alloc] init];
    NSMutableArray *account_ids = [[NSMutableArray alloc] init];
    NSMutableDictionary *accountDesc = [[NSMutableDictionary alloc] init];
    
    [accountDesc setObject:@"Add New Account" forKey:@"AccountTitle"];
    [accountDesc setObject:[UIImage imageNamed:@"people.png"] forKey:@"AccountImage"];
    
    [accounts addObject:accountDesc];
    [account_ids addObject:[NSNumber numberWithInteger:-1]];
    
   
    
    for (int i=0; i<acctList.size(); i++) {
        NSMutableDictionary *accountDesc = [[NSMutableDictionary alloc] init];
        ODAccount account = acctList[i];
        [accountDesc setObject:account.account_name.getPlatformString() forKey:@"AccountTitle"];
        std::string temp = account.icon_path.getUTF8String();
        
        //REVIST :(abedi). This should not have been needed. Also not a right place to do so.
        // Icon path should have been set by the service itself and ODAccount should have this information.
        NSString *iconPath = [[CommonUIUtils getInstance] getServiceIcon:account.account_service_id];
        
        [accountDesc setObject:[UIImage imageNamed:iconPath] forKey:@"AccountImage"];
        [accounts addObject:accountDesc];
        [account_ids addObject:[NSNumber numberWithInteger:account.account_id]];
        //        [self addToArray:kAccountsSection objectToAdd:account.account_name.getPlatformString()];
    }
    
    [dictionary setObject:accounts forKey:kObjectsString];
    [dictionary setObject:account_ids forKey:kObjectIdsString];
    
    NSIndexPath* indexPath = [NSIndexPath indexPathForRow:1 inSection:0];
    [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:0] withRowAnimation:UITableViewRowAnimationNone];
    [self.tableView selectRowAtIndexPath:indexPath animated:YES scrollPosition:UITableViewScrollPositionNone];
    [self.mSource callBackForLeftViewController:indexPath forObjectId:[NSNumber numberWithInteger:acctList[0].account_id]];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.tableView = [[UITableView alloc] initWithFrame: CGRectMake(0, 0, 200, 768) style:UITableViewStyleGrouped];
    
    self.tableView.rowHeight = kHeaderHeight;
    self.tableView.dataSource = self;
    self.tableView.delegate = self;
    self.tableView.tableFooterView.userInteractionEnabled = YES;
        self.tableView.scrollEnabled = NO;
    [ self.tableView setAutoresizingMask:(
                                        UIViewAutoresizingFlexibleHeight )];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateRecentView:) name:@"UpdateRecentView" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateAccountView:) name:@"UpdateAccountView" object:nil];
 
}

-(void) setSource : (id<leftTreeControllerDelegate>) source
{
    self.mSource = source;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    
    return [self.listOfItems count];
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    
    //Number of rows it should expect should be based on the section
    NSDictionary *dictionary = [listOfItems objectAtIndex:section];
    NSArray *array = [dictionary objectForKey:kObjectsString];
    return [array count];
}


- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    
    NSDictionary *dictionary = [listOfItems objectAtIndex:section];
    
    NSString *sectionTitle = [dictionary objectForKey:kTitleString];
    return sectionTitle;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    // Set up the cell...
    
    //First get the dictionary object
    NSDictionary *dictionary = [listOfItems objectAtIndex:indexPath.section];
    NSArray *array = [dictionary objectForKey:kObjectsString];
    NSString *cellValue;
    if(indexPath.section == 0) // Add Accounts
    {
        NSDictionary *dct = [array objectAtIndex:indexPath.row];
        cellValue = [dct objectForKey:@"AccountTitle"];
        cell.imageView.image = [dct objectForKey:@"AccountImage"];
    }
    else if(indexPath.section == 2) // recents
    {
        cellValue = [[array objectAtIndex:indexPath.row] getMediaName];
    }
    else
    {
        cellValue = [array objectAtIndex:indexPath.row];
    }
    
    cell.textLabel.text = cellValue;
    cell.textLabel.font = [UIFont fontWithName:@"Helvetica" size:14];
    
    return cell;
}


- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return  kCellHeight;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return kCellHeight;
}
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return kFooterHeight;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    
    NSDictionary *dictionary = [listOfItems objectAtIndex:section];
    
    NSString *sectionTitle = [dictionary objectForKey:kTitleString];
    
    UILabel *label = [[UILabel alloc] init];
    label.frame = CGRectMake(15, 0, 200, 40);
    label.textColor = [UIColor blackColor];
    label.font = [UIFont fontWithName:@"Helvetica" size:20];
    label.text = sectionTitle;
    label.backgroundColor = [UIColor clearColor];
    
    // Create header view and add label as a subview
    UIView *view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 40)];
    [view addSubview:label];
    
    return view;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    NSDictionary *dictionary = [listOfItems objectAtIndex:indexPath.section];
    NSArray *array = [dictionary objectForKey:kObjectIdsString];
    NSNumber *selectedObjectId = nil;
    if (array != nil) {
        selectedObjectId = [array objectAtIndex:indexPath.row];
    }
    if(indexPath.section == 2) // recents
    {
        NSArray *array = [dictionary objectForKey:kObjectsString];
        Media *md = [array objectAtIndex:indexPath.row];
        selectedObjectId = [[NSNumber alloc] initWithInt:md.media_id];
        recentMediaIDSelected = md.media_id;
        // Use this while querying then.
    }
    // Call the Callee
    [self.mSource callBackForLeftViewController:indexPath forObjectId:selectedObjectId];
    
    
}

@end
