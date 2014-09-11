//
//  SearchViewController.h
//  OneDrive
//
//  Created by Ajay Bedi on 18/12/12.
//  Copyright (c) 2012 Ajay Bedi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <UIKit/UICollectionViewController.h>
#import "Media.h"


@interface SearchViewController : UICollectionViewController <UICollectionViewDataSource, UICollectionViewDelegate>
{
    NSMutableArray *mediaObjs;
}
@property (retain, nonatomic) NSMutableArray *mediaObjs;
- (id) initSearchView: (CGRect) frame;
- (void) addMediaObject: (Media*) mediaObj;
- (void) cleanUp;

@end


enum SearchType
{
    SearchVideos,
    SearchImages,
    SearchPDF,
    SearchText,
    SearchFilterCount
};


@protocol SearchQueryDelegate
- (void) executeQuery:(NSString *)query;
@end


@interface SearchQueryController : UITableViewController <UITableViewDataSource, UITableViewDelegate> {
    NSMutableArray *_queries;
    id<SearchQueryDelegate> delegate;
}

@property (nonatomic, strong) NSMutableArray *_queries;
@property (nonatomic, weak) id<SearchQueryDelegate> delegate;

@end


@interface SearchToolBar : UIToolbar

-(void) setFocusOnSearch;
- (IBAction)ResetToolBar:(id)sender;

-(IBAction) setListView:(id)sender;
-(IBAction) setGridView:(id)sender;

- (IBAction)SearchAction:(id)sender;

@property (nonatomic, weak) id sender;

@property (nonatomic,retain) IBOutlet UIBarButtonItem *mDoneButton;
@property (strong, nonatomic) IBOutlet UISegmentedControl *mSegmentControl;
@property (nonatomic,retain)  IBOutlet UISearchBar *mSearchBar;
@property (nonatomic,retain)  IBOutlet UIBarButtonItem *mGridViewButton;
@property (nonatomic,retain)  IBOutlet UIBarButtonItem *mListViewButton;


@end




