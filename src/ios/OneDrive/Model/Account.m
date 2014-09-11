//
//  Account.m
//  OneDriveUI
//
//  Created by Vikash Jindal on 27/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "Account.h"

@implementation Account
- (id)initWithValue:(ODAccount) account{
	self = [super init];
	if (self) {
        _account_id = [NSNumber numberWithInt: account.account_id];
        _account_service_id = account.account_service_id;
        _account_name = account.account_name.getPlatformString();
        _account_token = account.account_token.getPlatformString();
        _icon_path = account.icon_path.getPlatformString();
        _account_status = account.account_status;
	}
	return self;
}
@end
