//
//  SearchViewController.m
//  OneDrive
//
//  Created by Ajay Bedi on 18/12/12.
//  Copyright (c) 2012 Ajay Bedi. All rights reserved.
//

#import "FavoritesViewController.h"
#import "SearchIconCell.h"
#import "AppController.h"
#import "RootViewController.h"

@implementation FavoritesViewController

void CallBackFavoritesMedia(RESULT res, ODMedia media, bool deleted, bool fromCache, id callerObject, id searchView)
{
    // Update the search View
    if(res == RESULT_NoErr)
    {
        Media *mediaObj = [[Media alloc] initWithValue:media];
        if([searchView isKindOfClass:[SearchViewController class]])
        {
            SearchViewController *tableViewController = (SearchViewController*)searchView;
            [tableViewController addMediaObject:mediaObj];
        }
        
    }
    
}

- (void)viewWillAppear:(BOOL)animated
{
    [self.searchViewController cleanUp];
    AppController &applicationCtr = AppController::instance();
    applicationCtr.cancelLastQuery();
    applicationCtr.GetFavoriteMedia(std::bind(CallBackFavoritesMedia, std::placeholders::_1,
                                              std::placeholders::_2, std::placeholders::_3, YES, self,self.searchViewController));

    
}
-(void) viewDidLoad
{
    [super viewDidLoad];
    // Fire a Favorites Query and Set the view of Search Result holder.
    
    self.searchViewController = [[SearchViewController alloc] initSearchView: self.centerView.frame];
    
    [self.searchViewController collectionView].delegate = self.searchViewController;
    [self.searchViewController collectionView].dataSource = self.searchViewController;
    [self.centerView addSubview:[self.searchViewController collectionView]];
    
    }

@end



@implementation FavoritesToolbar
@synthesize sender;
@synthesize filtersText;

-(IBAction) setListView:(id)sBar
{
    [(RootViewController*)sender setListView:self];
}
-(IBAction) setGridView:(id)sBar
{
    [(RootViewController*)sender setGridView:self];
    
}

-(void) setFiltersText:(NSString *)text
{

    [(UILabel*)self.filtersText.customView setText:text];
   // [self.filtersText setTitle:text];
}

@end
