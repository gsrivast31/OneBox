//
//  ODPreferences.m
//  OneDrive
//
//  Created by Gaurav Srivastava on 6/17/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import "ODPreferences.h"

@implementation ODPreferences

-(BOOL) getAccountSyncState:(ODAccount)account {
    return TRUE;
}

-(void) setAccountSyncState:(ODAccount)account state:(BOOL)on {
    
}

-(BOOL) getAccountSignInState:(ODAccount)account {
    return TRUE;
}

-(void) setAccountSignInState:(ODAccount)account state:(BOOL)on {
    
}

-(eSiblingOrderPolicy) getDefaultSortOrder {
    return kOrderByName;
}

-(void) setDefaultSortOrder:(eSiblingOrderPolicy)order {
    
}

-(BOOL) getVerboseLoggingEnabled {
    return TRUE;    
}

-(void) setVerboseLoggingEnabled:(BOOL)on {
    
}

-(int) getAccountSyncInterval {
    return 10;
}

-(void) setAccountSyncInterval:(int)interval {
    
}

@end
