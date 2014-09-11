//
//  leftTreeViewController.h
//  OneDrive_temp
//
//  Created by Ajay on 4/14/13.
//  Copyright (c) 2013 Ajay. All rights reserved.
//

#import <UIKit/UITableViewController.h>


@protocol leftTreeControllerDelegate <NSObject>

- (void) callBackForLeftViewController : (NSIndexPath*)indexPath forObjectId:(NSNumber*)objectId;

@end

@interface leftTreeViewController : UITableViewController <UITableViewDataSource, UITableViewDelegate>
{
    int recentMediaIDSelected;
}
- (id)initWithStyle:(UITableViewStyle)style;
-(void) setSource : (id<leftTreeControllerDelegate>) source;
-(void) setItems : (NSArray *) items;

@property (nonatomic, strong) NSArray *listOfItems;
@property (nonatomic, strong) UITableView *mtableView;
@property ( nonatomic, weak) id < leftTreeControllerDelegate> mSource;


@end
