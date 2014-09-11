//
//  PopoverTableViewController.m
//  OneDrive
//
//  Created by Vikash Jindal on 19/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "PopoverTableViewController.h"
#import "PopupBase.h"

@interface PopoverTableViewController ()

@end

static NSInteger const kCellHeight  = 40;
static NSInteger const kNavigationbarHeight  = 30;
static NSInteger const kSectionHeaderHeight  = 4;
static NSInteger const kSectionFooterHeight  = 2;

@implementation PopoverTableViewController

@synthesize sectionsArray;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void) initSectionArray:(NSInteger)sections
{
    sectionsArray = [[NSMutableArray alloc] init];
    for (NSInteger i = 0; i < sections; i++) {
        [sectionsArray addObject:[NSMutableArray array]];
    }
}

- (void) addToArray:(NSInteger)index objectToAdd:(id)object
{
    [[self.sectionsArray objectAtIndex:index] addObject:object];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    ((UITableView*)(self.view)).rowHeight = kCellHeight;
    ((UITableView*)(self.view)).sectionHeaderHeight = kSectionHeaderHeight;
    ((UITableView*)(self.view)).sectionFooterHeight = kSectionFooterHeight;
    
    NSUInteger tableHeight;
    tableHeight = kNavigationbarHeight;
    
    for (NSInteger i = 0; i < [sectionsArray count]; i++) {
        tableHeight += kSectionHeaderHeight + kSectionFooterHeight + kCellHeight * [[sectionsArray objectAtIndex:i] count];
    }
        
    self.view.frame = CGRectMake(0, 0, 250, tableHeight);
    self.contentSizeForViewInPopover = CGSizeMake(self.view.bounds.size.width, tableHeight);
    
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
-(void) dealloc
{
    _delegate = nil;
    
}
#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return [self.sectionsArray count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [[self.sectionsArray objectAtIndex:section] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSUInteger section = [indexPath section];
	NSUInteger row = [indexPath row];
	
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    if( indexPath.row == _selected)
    {
        cell.accessoryType=UITableViewCellAccessoryCheckmark;
    }
    else{
        cell.accessoryType = UITableViewCellAccessoryNone;
    }
    
    PopupBase* item = [[self.sectionsArray objectAtIndex:section] objectAtIndex:row];
    cell.textLabel.text = item.name;
    cell.imageView.image = [UIImage imageNamed:item.image];
    
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



- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(_selected != -1)
    {
        UITableViewCell *cell1 = [tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:indexPath.row inSection:_selected]];
        cell1.accessoryType = UITableViewCellAccessoryNone;
        cell1 = [tableView cellForRowAtIndexPath:indexPath];
        cell1.accessoryType = UITableViewCellAccessoryCheckmark;
        _selected = indexPath.section;
    }
//    if([_delegate respondsToSelector:@selector(takenInputAtIndex::)])
    PopupBase* item = [[self.sectionsArray objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
        [_delegate takenInputAtIndex:self selectedIndex:item.index ];
}
@end
