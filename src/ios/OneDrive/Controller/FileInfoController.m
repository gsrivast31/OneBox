//
//  FileInfoController.m
//  OneDrive
//
//  Created by Gaurav Srivastava on 4/16/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import "FileInfoController.h"

static NSInteger const kTitle = 0;
static NSInteger const kValue  = 1;
static NSInteger const kCellHeight  = 30;
static NSInteger const kNavigationbarHeight  = 0;
static NSInteger const kSectionHeaderHeight  = 15;
static NSInteger const kSectionFooterHeight  = 15;

@interface FileInfoController ()

@end

@implementation FileInfoController

@synthesize rowsArray;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void) setMediaNode:(ODMedia)med
{
    self->media = med;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self setUpRowsArray];
    
    ((UITableView*)(self.view)).rowHeight = kCellHeight;
    ((UITableView*)(self.view)).sectionHeaderHeight = kSectionHeaderHeight;
    ((UITableView*)(self.view)).sectionFooterHeight = kSectionFooterHeight;
    
    NSUInteger tableHeight = 0;
    //tableHeight = kNavigationbarHeight;
    
    tableHeight += kSectionHeaderHeight + kSectionFooterHeight + kCellHeight * [[rowsArray objectAtIndex:0] count];
    
    self.view.frame = CGRectMake(0, 0, 200, tableHeight);
    self.contentSizeForViewInPopover = CGSizeMake(self.view.bounds.size.width, tableHeight);

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void) addToArray:(NSInteger)index objectToAdd:(id)object
{
    [[self.rowsArray objectAtIndex:index] addObject:object];
}

- (void) setUpRowsArray
{
    self.rowsArray = [NSMutableArray arrayWithObjects:
                          [NSMutableArray array], //title section
                          [NSMutableArray array], //value section
                          nil];
    
    [self addToArray:kTitle objectToAdd:@"Path"];
    [self addToArray:kValue objectToAdd:media.file_name.getPlatformString()];

    NSString* sizeStr = [NSString stringWithFormat:@"%lld",media.size];
    sizeStr = [sizeStr stringByAppendingString:@" bytes"];
    [self addToArray:kTitle objectToAdd:@"Size"];
    [self addToArray:kValue objectToAdd:sizeStr];
    
    [self addToArray:kTitle objectToAdd:@"Date"];
    [self addToArray:kValue objectToAdd:media.mod_date.ToString().getPlatformString()];
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
    return [[self.rowsArray objectAtIndex:section] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSUInteger row = [indexPath row];
	
    static NSString *CellIdentifier = @"InfoCell";
    
    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier];
    }
    
    cell.textLabel.text = [[self.rowsArray objectAtIndex:0] objectAtIndex:row];
    cell.textLabel.font = [UIFont boldSystemFontOfSize:15.0f];
    
    cell.detailTextLabel.text = [[self.rowsArray objectAtIndex:1] objectAtIndex:row];
    cell.detailTextLabel.font = [UIFont italicSystemFontOfSize:16.0f];
    
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    // Configure the cell...
    
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
