//
//  ProgressViewController.h
//  OneDrive
//
//  Created by Ruchin Kansal on 28/05/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ProgressViewController : UIViewController

@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;
@property (weak, nonatomic) IBOutlet UILabel *progressLabel;
@property (strong, nonatomic) NSString* progressText;
@end

