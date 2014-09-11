//
//  MasterViewController.m
//  OneDrive
//
//  Created by Gaurav Srivastava on 3/18/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import "MasterViewController.h"
#import "SplitViewController.h"
#import "DetailViewController.h"
#import "AccountViewController.h"
#import "AppController.h"
#import "CommonUIUtils.h"
#import "Media.h"

@interface MasterViewController ()

@end

static NSInteger const kNumberOfSections  = 3;
static NSInteger const kAccountsSection  = 0;
static NSInteger const kFiltersSection  = 1;
static NSInteger const kRecentsSection  = 2;
static NSInteger const kCellHeight  = 40;
static NSInteger const kNavigationbarHeight  = 30;
static NSInteger const kSectionHeaderHeight  = 4;
static NSInteger const kSectionFooterHeight  = 2;

@implementation MasterViewController

@synthesize sectionsArray;
@synthesize splitViewController;
@synthesize sectionsData;
@synthesize cntr;

-(id)initWithSplitViewController:(SplitViewController*)splitViewCtrl
{
    self = [super init];
    if(self){
        splitViewController = splitViewCtrl;
    }
    
    return self;
}

- (void) addToArray:(NSInteger)index objectToAdd:(id)object
{
    [[self.sectionsArray objectAtIndex:index] addObject:object];
}

- (void)settingsClicked {
    UIViewController *settingController = [self.splitViewController.storyboard instantiateViewControllerWithIdentifier:@"SettingsView"];
    
    UIBarButtonItem *doneButton = [[UIBarButtonItem alloc] initWithTitle:@"Done" style:UIBarButtonItemStyleDone target:self action:@selector(doneAction:)];
	settingController.navigationItem.leftBarButtonItem = doneButton;

    UINavigationController *controller = [[UINavigationController alloc] initWithRootViewController:settingController];
    controller.modalPresentationStyle = UIModalPresentationFormSheet;
    controller.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    [self presentViewController:controller animated:YES completion:nil];

    settingController.view.superview.frame = CGRectMake(0, 0, 600, 600);//it's important to do this after
    settingController.view.superview.center = self.splitViewController.view.center;
}

- (void)doneAction:(id)sender
{
	[self dismissViewControllerAnimated:YES completion:nil];
}

- (void) setUpSectionsArray
{
    UIToolbar *toolbar = [[UIToolbar alloc] init];
    toolbar.frame = CGRectMake(0, 0, self.view.frame.size.width, 44);

    NSMutableArray* BarbuttonsArray = [[NSMutableArray alloc] init];

    UIBarButtonItem * logoButton = [[UIBarButtonItem alloc] initWithTitle:@"OneDrive" style:UIBarButtonItemStylePlain target:nil action:nil];
    [BarbuttonsArray  addObject:logoButton];

    UIBarButtonItem *fixedSpace = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
    [BarbuttonsArray  addObject:fixedSpace];
    
    UIBarButtonItem * settingsButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"gear-mini.png"] style:                        UIBarButtonItemStylePlain target:self action:@selector(settingsClicked)];
    [BarbuttonsArray  addObject:settingsButton];

    [fixedSpace setWidth:60.0f];
    
    [toolbar setItems:BarbuttonsArray  animated:NO];

    [self.view addSubview:toolbar];
    
    // Do any additional setup after loading the view, typically from a nib.
    cntr = [[leftViewController alloc] initWithSplitViewController:self.splitViewController];
    
    sectionsData = [[NSMutableArray alloc] init];
    
    // First Section ( Add Accounts)
    NSMutableArray *accounts = [[NSMutableArray alloc] init];
    NSMutableArray *account_ids = [[NSMutableArray alloc] init];
    
    NSMutableDictionary *accountDesc = [[NSMutableDictionary alloc] init];
    
    [accountDesc setObject:@"Add New Account" forKey:@"AccountTitle"];
    [accountDesc setObject:[UIImage imageNamed:@"people.png"] forKey:@"AccountImage"];
    
    [accounts addObject:accountDesc];
    [account_ids addObject:[NSNumber numberWithInteger:-1]];
    
    AppController &applicationCtr = AppController::instance();
    ODAccountList registered_accounts;
    applicationCtr.GetAllAccounts(registered_accounts);
    
    for (int i=0; i<registered_accounts.size(); i++) {
        NSMutableDictionary *accountDesc = [[NSMutableDictionary alloc] init];
        ODAccount account = registered_accounts[i];
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
    
    if (registered_accounts.size() > 0) {
        [[CommonUIUtils getInstance] setActiveAccountID:[NSNumber numberWithInteger:registered_accounts[0].account_id]];
    }
    else {
        [[CommonUIUtils getInstance] setActiveAccountID:[NSNumber numberWithInteger:0]];
    }

    
    NSMutableDictionary *dictionary = [[NSMutableDictionary alloc] init];
    [dictionary setObject:@"Accounts" forKey:@"Title"];
    [dictionary setObject:accounts forKey:@"Objects"];
    [dictionary setObject:account_ids forKey:@"ObjectIds"];
    
    // Second Section ( Filters) . These will be more or less same (hard coded)
    
    NSMutableArray *filters = [[NSMutableArray alloc] init];
    [filters addObject:@"Photos"];
    [filters addObject:@"Videos"];
    [filters addObject:@"Documents"];
    [filters addObject:@"PDF"];
    [filters addObject:@"Favorites"];
    
    
    NSMutableDictionary *filtersdictionary = [[NSMutableDictionary alloc] init];
    [filtersdictionary setObject:@"Filters" forKey:@"Title"];
    [filtersdictionary setObject:filters forKey:@"Objects"];
    
    // Third Section (Recent)
    
    ODMediaList recentList;
    applicationCtr.GetRecents(recentList);
    
    NSMutableArray *recents = [[NSMutableArray alloc] init];
    for (int i=0; i<recentList.size(); i++) {
        ODMedia media = recentList[i];
        [recents addObject:[[Media alloc ]initWithValue:media]];
    }

    //TODO: Query for the recent documents and add them.
    
    NSMutableDictionary *recentsDictionary = [[NSMutableDictionary alloc] init];
    [recentsDictionary setObject:@"Recents" forKey:@"Title"];
    [recentsDictionary setObject:recents forKey:@"Objects"];
    
    [sectionsData addObject:dictionary];
    [sectionsData addObject:filtersdictionary];
    [sectionsData addObject:recentsDictionary];
    
    [cntr setTableData:sectionsData];
    cntr.view.frame = CGRectMake(0, 44, self.view.frame.size.width, self.view.frame.size.height - 44);
    //cntr.view.frame = self.view.frame;
    [self.view addSubview:cntr.view];

}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self setUpSectionsArray];
    
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
    // Return the number of sections.
    return [self.sectionsArray count];
}

@end
