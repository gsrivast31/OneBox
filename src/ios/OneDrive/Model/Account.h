//
//  Account.h
//  OneDriveUI
//
//  Created by Vikash Jindal on 27/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "ODDataController.h"

@interface Account : NSObject

@property (nonatomic, strong) NSNumber *account_id;
@property (nonatomic) int account_service_id;
@property (nonatomic, strong) NSString *account_name;
@property (nonatomic, strong) NSString * account_token;
@property (nonatomic, strong) NSString * icon_path;
@property (nonatomic)bool account_status;
@property (nonatomic)long timestamp;
- (id)initWithValue:(ODAccount) account;

@end
