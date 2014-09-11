//
//  MasterViewController.h
//  OneDrive
//
//  Created by Gaurav Srivastava on 3/18/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "leftView.h"

@class DetailViewController;
@class SplitViewController;

@interface MasterViewController : UIViewController
{
    NSMutableArray*	_sectionsArray;
}
@property (nonatomic, retain) NSMutableArray *sectionsData;
@property (nonatomic,retain) leftViewController *cntr;
@property (nonatomic, retain) NSMutableArray    *sectionsArray;
@property (strong, nonatomic) DetailViewController *detailViewController;
@property (nonatomic, retain) SplitViewController *splitViewController;

- (id) initWithSplitViewController:(SplitViewController*)splitViewController;
- (void) initSectionArray:(NSInteger)section;
- (void) addToArray:(NSInteger)index objectToAdd:(id)object;
- (IBAction)settingsClicked:(id)sender;

@end
