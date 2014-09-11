//
//  AccountViewController.h
//  OneDriveUI
//
//  Created by Gaurav Srivastava on 3/12/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AccountViewController : UITableViewController
{
    NSMutableArray*	_servicesArray;
}

@property (nonatomic, retain) NSMutableArray    *servicesArray;

- (void) setUpServicesArray;
- (IBAction)close:(id)sender;
@end
