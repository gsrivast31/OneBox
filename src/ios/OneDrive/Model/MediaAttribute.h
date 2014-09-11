//
//  MediaAttribute.h
//  OneDriveUI
//
//  Created by Vikash Jindal on 28/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TreeNode.h"
@interface MediaAttribute : NSObject
@property (nonatomic, strong) TreeNode *root;
@property (nonatomic, strong) NSString *path;
-(id) initWithValue:(TreeNode*) node;
@end
