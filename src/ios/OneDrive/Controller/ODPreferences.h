//
//  ODPreferences.h
//  OneDrive
//
//  Created by Gaurav Srivastava on 6/17/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AppController.h"

@interface ODPreferences : NSObject

-(BOOL) getAccountSyncState:(ODAccount)account;
-(void) setAccountSyncState:(ODAccount)account state:(BOOL)on;

-(BOOL) getAccountSignInState:(ODAccount)account;
-(void) setAccountSignInState:(ODAccount)account state:(BOOL)on;

-(eSiblingOrderPolicy) getDefaultSortOrder;
-(void) setDefaultSortOrder:(eSiblingOrderPolicy)order;

-(BOOL) getVerboseLoggingEnabled;
-(void) setVerboseLoggingEnabled:(BOOL)on;

-(int) getAccountSyncInterval;
-(void) setAccountSyncInterval:(int)interval;

@end
