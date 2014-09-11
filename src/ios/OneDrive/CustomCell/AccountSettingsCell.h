//
//  AccountSettingsCell.h
//  OneDriveUI
//
//  Created by Gaurav Srivastava on 3/11/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AccountSettingsCell : UITableViewCell

@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
@property (weak, nonatomic) IBOutlet UILabel *storageLabel;
@property (weak, nonatomic) IBOutlet UISwitch *status;
@property (weak, nonatomic) IBOutlet UIImageView *imageView;

@end
