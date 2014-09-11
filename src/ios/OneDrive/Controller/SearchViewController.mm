//
//  SearchViewController.m
//  OneDrive
//
//  Created by Ajay Bedi on 18/12/12.
//  Copyright (c) 2012 Ajay Bedi. All rights reserved.
//

#import "SearchViewController.h"
#import "SearchIconCell.h"
#import "RootViewController.h"

@implementation SearchViewController

@synthesize mediaObjs;

- (id)initSearchView:(CGRect) frame
{
    self = [super init];
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    layout.itemSize = CGSizeMake(72, 135);
    layout.minimumInteritemSpacing = 10;
    layout.minimumLineSpacing = 10;
    layout.scrollDirection = UICollectionViewScrollDirectionVertical;
    layout.sectionInset = UIEdgeInsetsMake(10, 10, 10, 10);
    UICollectionView *collectionView = [[UICollectionView alloc] initWithFrame:frame collectionViewLayout:layout];
    
    self.collectionView = collectionView;
    self.collectionView.delegate = self;
    self.collectionView.dataSource = self;
    self.collectionView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.jpg"]];
     mediaObjs = [[NSMutableArray alloc] init];
    UINib *cellNib = [UINib nibWithNibName:@"SearchIconCell" bundle:nil];
    [self.collectionView registerNib:cellNib forCellWithReuseIdentifier:@"SCON"];
 //   [self.collectionView registerClass:[SearchIconCell class] forCellWithReuseIdentifier:@"SCON"];

     return self;

}
- (void) addMediaObject:(Media *)mediaObj
{
    [mediaObjs addObject:mediaObj];
    [self.collectionView reloadData];
}

- (void) cleanUp
{
    [mediaObjs removeAllObjects];
    [self.collectionView reloadData];
}

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return [self.mediaObjs count];
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView1 cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    SearchIconCell *icon = [self.collectionView dequeueReusableCellWithReuseIdentifier:@"SCON" forIndexPath:indexPath];
    Media *obj = (Media *)[self.mediaObjs objectAtIndex:indexPath.row];
    icon.mAssetName.text = [NSString stringWithFormat:@"%@",obj.name];//parsedName;
    icon.mSize.text = [NSString stringWithFormat:@"%lld Bytes",obj.size];
    icon.mDate.text = @"12/03/2013";
    if(obj.icon == nil)
            icon.mThumbnail.image = [UIImage imageNamed:@"folder"] ;
    else
    {
            icon.mThumbnail.image = [UIImage imageWithContentsOfFile:obj.icon];
            if(icon.mThumbnail.image == nil)
            {
                icon.mThumbnail.image = [UIImage imageNamed:obj.icon];
            }
    }
  
    // Set up the cell properly... (TODO)
    return icon;
}

@end



@implementation SearchQueryController

@synthesize _queries;

- (void)viewDidLoad {
    [super viewDidLoad];
    self.clearsSelectionOnViewWillAppear = NO;
    self.contentSizeForViewInPopover = CGSizeMake(150.0, 220.0);
    self._queries = [NSMutableArray array];
    [_queries addObject:@"Images"];
    [_queries addObject:@"Videos"];
    [_queries addObject:@"PDF"];
    [_queries addObject:@"Text"];
    [_queries addObject:@"Other"];
    self.tableView.delegate = self;
    self.tableView.dataSource = self;

}


- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Return the number of sections.
    return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    // Return the number of rows in the section.
    return [self._queries count];
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    // Configure the cell...
    NSString *txt = [_queries objectAtIndex:indexPath.row];
    cell.textLabel.text = txt;
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if (_delegate != nil) {
        NSString *txt = [_queries objectAtIndex:indexPath.row];
        [_delegate executeQuery:txt];
    }
}

@end


@implementation SearchToolBar

@synthesize mSearchBar;
@synthesize mDoneButton;
@synthesize mGridViewButton;
@synthesize mListViewButton;
@synthesize sender;
@synthesize mSegmentControl;


-(void) setFocusOnSearch
{
    [mSearchBar becomeFirstResponder];
}

-(IBAction) setListView:(id)sBar
{
    [(RootViewController*)sender setListView:self];
}
-(IBAction) setGridView:(id)sBar
{
    [(RootViewController*)sender setGridView:self];

}

- (IBAction)SearchAction:(id)segmentCtrl
{
   [(RootViewController*)sender reloadQuery];
}


- (IBAction)ResetToolBar:(id) sBar
{
    [(RootViewController*)sender resetToolBar:self];
}

@end



