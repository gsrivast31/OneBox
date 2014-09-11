//
//  UploadPopoverTableViewController.m
//  Test
//
//  Created by labuser on 3/7/13.
//  Copyright (c) 2013 labuser. All rights reserved.
//

#import "UploadPopoverTableViewController.h"
#import "FileUploadManager.h"

static NSInteger const kNumberOfSections  = 2;
static NSInteger const kGeneralSection  = 0;
static NSInteger const kUploadSection  = 1;
static NSInteger const kUploadStatusSection  = 2;
static NSInteger const kCellHeight  = 40;
static NSInteger const kNavigationbarHeight  = 60;
static NSInteger const kSectionHeaderHeight  = 40;
static NSInteger const kSectionFooterHeight  = 40;
static NSInteger const kUploadCellHeight = 82;

@implementation UploadPopoverTableViewController

@synthesize sectionsArray;
@synthesize folderName;

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
    
    self.title = NSLocalizedString(@"Add", @"nav bar title");
    
    _tableHeight = kNavigationbarHeight;
    
    [self setUpSectionsArray];
    
    ((UITableView*)(self.view)).rowHeight = kCellHeight;
    
    self.view.frame = CGRectMake(0, 0, 300, _tableHeight);
    self.contentSizeForViewInPopover = CGSizeMake(self.view.bounds.size.width, _tableHeight);
}

- (void) addToArray:(NSInteger)index objectToAdd:(id)object heightOfRow:(NSInteger)height
{
    [[self.sectionsArray objectAtIndex:index] addObject:object];
    _tableHeight += height;
}

- (void) setUpSectionsArray
{
    self.sectionsArray = [NSMutableArray arrayWithObjects:
                          [NSMutableArray array], //general section
                          [NSMutableArray array], //upload section
                          [NSMutableArray array], //upload status section
                          nil];
    
    _tableHeight += kSectionHeaderHeight;
    
    [self addToArray:kGeneralSection objectToAdd:@"Add Folder" heightOfRow:kCellHeight];
    
    [self addToArray:kUploadSection objectToAdd:@"Upload from Gallery" heightOfRow:kCellHeight];
    [self addToArray:kUploadSection objectToAdd:@"Use Camera" heightOfRow:kCellHeight];
    if(  FileUploadProgressManager::instance().TotalMediaUploading() > 0)
        [self addToArray:kUploadStatusSection objectToAdd:@"Show Uploading Media" heightOfRow:kCellHeight];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return [self.sectionsArray count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self.sectionsArray objectAtIndex:section] count];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    
	if(section == kGeneralSection) {
        if ([folderName length] > 0) {
            NSString* title = @"To folder: ";
            title = [title stringByAppendingString:folderName];
            return title;
        }
	}
	return @"";
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSUInteger section = [indexPath section];
	NSUInteger row = [indexPath row];
	
    static NSString *CellIdentifier = @"FolderCell";
    
    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    cell.textLabel.text = [[self.sectionsArray objectAtIndex:section] objectAtIndex:row];
    
    if (indexPath.section == kGeneralSection && indexPath.row == 0) {
        cell.imageView.image = [UIImage imageNamed:@"folder-add-mini.png"];
    }
     else if (indexPath.section == kUploadSection && indexPath.row == 0) {
        cell.imageView.image = [UIImage imageNamed:@"arrow-out-mini.png"];
    }
    else if (indexPath.section == kUploadSection && indexPath.row == 1) {
        cell.imageView.image = [UIImage imageNamed:@"camera-mini.png"];
    }
    else if(indexPath.section == kUploadStatusSection && indexPath.row == 0)
    {
        cell.imageView.image = [UIImage imageNamed:@"camera-mini.png"];
    }
    
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
    if (indexPath.section == kUploadSection && indexPath.row == 0)
    {
        if ([_delegate respondsToSelector:@selector(openGallery)]) {
            [_delegate openGallery];
        }
    }
    else if (indexPath.section == kUploadSection && indexPath.row == 1)
    {
        if ([_delegate respondsToSelector:@selector(openCamera)]) {
            [_delegate openCamera];
        }
    }
    else if (indexPath.section == kGeneralSection && indexPath.row == 0)
    {
        if ([_delegate respondsToSelector:@selector(openAddFolderWorkflow)]) {
            [_delegate openAddFolderWorkflow];
        }
    }
    else if (indexPath.section == kUploadStatusSection && indexPath.row == 0)
    {
        UploadMediaStatusController *cntr = [[UploadMediaStatusController alloc] init];
        [self.navigationController pushViewController:cntr animated:YES];
    }
}

@end






@implementation UploadMediaStatusController
@synthesize sectionsArray;



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
    
    self.title = NSLocalizedString(@"Upload Status", @"nav bar title");
    
    _tableHeight = kNavigationbarHeight;
    [self setUpSectionsArray];
    
    ((UITableView*)(self.view)).rowHeight = kUploadCellHeight;
    self.view.frame = CGRectMake(0, 0, 300, _tableHeight);
    self.contentSizeForViewInPopover = CGSizeMake(self.view.bounds.size.width, _tableHeight);
    
   // UINib *cellNib = [UINib nibWithNibName:@"uploadStatus" bundle:nil];
   // [self.tableView registerNib:cellNib forCellWithReuseIdentifier:@"UPCELL"];
    
    [self.tableView registerNib:[UINib nibWithNibName:@"uploadStatus" bundle:nil] forCellReuseIdentifier:@"UPCELL"];

}

- (void) addToArray:(NSInteger)index objectToAdd:(id)object heightOfRow:(NSInteger)height
{
    [[self.sectionsArray objectAtIndex:index] addObject:object];
    _tableHeight += height;
    
}

- (void) setUpSectionsArray
{
    self.sectionsArray = [NSMutableArray arrayWithObjects:
                          [NSMutableArray array], //general section
                          nil];
    
     _tableHeight += kSectionHeaderHeight;
    FileUploadProgressManager::UploadFileNodes mediaNodes = FileUploadProgressManager::instance().GetUploadMediaNodes();
    
    FileUploadProgressManager::UploadFileNodes::iterator iter = mediaNodes.begin();
    while(iter != mediaNodes.end())
    {
        std::pair<UString, FileUploadProgressManager::UploadInfoData_t> node(*iter);
        UString filePath = node.first;
        NSString *path = filePath.getPlatformString();
        UploadStatusInfo *info = [[UploadStatusInfo alloc] init];
        info.localPath = path;
        info.name = [[path lastPathComponent] stringByDeletingPathExtension];
      
        // Fix sizes when available from service end.
        info.sizeCompleted = [[NSNumber alloc] initWithInt:1];
        info.totalSize = [[NSNumber alloc ] initWithInt:10];
        [self addToArray:0 objectToAdd:info heightOfRow:kUploadCellHeight];
        iter++;
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return [self.sectionsArray count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self.sectionsArray objectAtIndex:section] count];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    
	return @"";
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    //Fix this...
    NSUInteger section = [indexPath section];
	NSUInteger row = [indexPath row];
	
    static NSString *CellIdentifier = @"UPCELL";
    
    UploadStatusCell* cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UploadStatusCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    UploadStatusInfo *info = (UploadStatusInfo*)[[self.sectionsArray objectAtIndex:section] objectAtIndex:row];
    cell.imgView.image = [UIImage imageWithContentsOfFile:info.localPath];
    cell.name.text = info.name;
    [cell.prgressView startAnimating];
    return cell;
}

@end

@implementation UploadStatusInfo
@synthesize localPath;
@synthesize name;
@synthesize totalSize;
@synthesize sizeCompleted;


@end

@implementation UploadStatusCell


@end