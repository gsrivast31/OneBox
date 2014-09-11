//
//  AccountSettingsCell.m
//  OneDriveUI
//
//  Created by Gaurav Srivastava on 3/11/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import "AccountSettingsCell.h"

@implementation AccountSettingsCell

@synthesize nameLabel;
@synthesize storageLabel;
@synthesize status;
@synthesize imageView;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
