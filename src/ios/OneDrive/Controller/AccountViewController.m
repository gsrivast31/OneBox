//
//  AccountViewController.m
//  OneDriveUI
//
//  Created by Gaurav Srivastava on 3/12/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import "AccountViewController.h"
#import "AddAccountViewController.h"
#import "MasterDetailViewManager.h"
#import "CommonUIUtils.h"
#import "WrappedAccountData.h"
@interface AccountViewController ()

@end

static NSInteger const kCellHeight  = 50;
static NSInteger const kNavigationbarHeight  = 60;
static NSInteger const kSectionHeaderHeight  = 1;
static NSInteger const kSectionFooterHeight  = 8;

@implementation AccountViewController

@synthesize servicesArray;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void) addToArray:(NSInteger)index objectToAdd:(id)object
{
    [[self.servicesArray objectAtIndex:index] addObject:object];
}

- (void)setUpServicesArray
{
    self.servicesArray = [NSMutableArray arrayWithObjects:
                          [NSMutableArray array], //title section
                          [NSMutableArray array], //image section
                          nil];
    
    [self addToArray:0 objectToAdd:@"Dropbox" ];
    [self addToArray:1 objectToAdd:[[CommonUIUtils getInstance] getServiceIcon:kSPDropbox] ];

    [self addToArray:0 objectToAdd:@"Google Drive" ];
    [self addToArray:1 objectToAdd:[[CommonUIUtils getInstance] getServiceIcon:kSPGDrive] ];

    [self addToArray:0 objectToAdd:@"SkyDrive" ];
    [self addToArray:1 objectToAdd:[[CommonUIUtils getInstance] getServiceIcon:kSPSkyDrive] ];
}

- (IBAction)close:(id)sender {
/*    [self.navigationController popToRootViewControllerAnimated: YES];

    if([sender isKindOfClass:[WrappedAccountData class]])
    {
        NSNotification *notification = [NSNotification notificationWithName:@"UpdateAccountView" object:nil];
        [[NSNotificationCenter defaultCenter] postNotification:notification];

        //if no error post a notification ot fetch the data in rootview controller
        NSNotification *notify = [NSNotification notificationWithName:@"AccountFetched" object:sender];
        [[NSNotificationCenter defaultCenter] postNotification:notify];
    }*/
}

-(void) dismiss:(NSNotification*)notificaion
{
    [self performSelectorOnMainThread:@selector(close:) withObject:[notificaion object] waitUntilDone:YES];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.title = NSLocalizedString(@"Add Account", @"nav bar title");
    
    [self setUpServicesArray];
    
    ((UITableView*)(self.view)).rowHeight = kCellHeight;
    ((UITableView*)(self.view)).sectionHeaderHeight = kSectionHeaderHeight;
    ((UITableView*)(self.view)).sectionFooterHeight = kSectionFooterHeight;
    
    UIView* bgView = [[UIView alloc] init];
    bgView.frame = self.view.frame;
    bgView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.jpg"]];
    ((UITableView*)(self.view)).backgroundView = bgView;
    
//    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(dismiss:) name:@"AccountAddResponse" object:nil];

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self.servicesArray objectAtIndex:0] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSUInteger row = [indexPath row];
	
    static NSString *CellIdentifier = @"AddAccountCell";
    
    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    cell.textLabel.text = [[self.servicesArray objectAtIndex:0] objectAtIndex:row];
    cell.imageView.image = [UIImage imageNamed:[[self.servicesArray objectAtIndex:1] objectAtIndex:row]];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    
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
    AddAccountViewController *targetVC = [[AddAccountViewController alloc] init];
    targetVC.name = @"TEST";
    targetVC.serviceID = kSPDropbox;
    targetVC.childview = self.view;

    
    // Handling 3 services for the start. Will add more eventually.
    if(indexPath.row == 1)
         targetVC.serviceID = kSPGDrive;
    else if(indexPath.row == 2)
         targetVC.serviceID = kSPSkyDrive;
        
    
    [self.navigationController pushViewController:targetVC animated:YES];
    
    /*AddAccountViewController *targetVC = [[AddAccountViewController alloc] init];
    targetVC.name = @"TEST";
    targetVC.serviceID = kSPDropbox;
    targetVC.childview = self.view;
    
    [self.navigationController pushViewController:targetVC animated:YES];*/
    
}

@end
