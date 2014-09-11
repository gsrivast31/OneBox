//
//  MediaAttribute.m
//  OneDriveUI
//
//  Created by Vikash Jindal on 28/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "MediaAttribute.h"
@interface MediaAttribute()

@end

@implementation MediaAttribute
- (id)initWithValue:(TreeNode*) node{
	self = [super init];
	if (self) {
        _root = node;
        
    }
	
	return self;
}

@end
