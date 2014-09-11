//
//  SearchViewController.h
//  OneDrive
//
//  Created by Ajay Bedi on 18/12/12.
//  Copyright (c) 2012 Ajay Bedi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <UIKit/UIViewController.h>
#import "SearchViewController.h"
#import "Media.h"


@interface FavoritesViewController : UIViewController

@property (weak, nonatomic) IBOutlet UIBarButtonItem *editButton;
@property (weak, nonatomic) IBOutlet UIView * centerView;
@property (strong, nonatomic) SearchViewController *searchViewController;
@end

@interface FavoritesToolbar : UIToolbar

-(IBAction) setListView:(id)sender;
-(IBAction) setGridView:(id)sender;

@property (nonatomic, weak) id sender;
@property (nonatomic, strong) IBOutlet UIBarButtonItem *filtersText;


@end