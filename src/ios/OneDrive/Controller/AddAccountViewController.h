//
//  AddAccountViewController.h
//  OneDrive
//
//  Created by Vikash Jindal on 19/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "AppController.h"
#import "MasterDetailViewManager.h"

@interface AddAccountViewController : UIViewController
{
    
}
@property (strong, nonatomic) NSString* name;;
@property (nonatomic) ServiceProvider serviceID;
@property (strong, nonatomic) UIView *childview;

- (IBAction)close:(id)sender;

@end
