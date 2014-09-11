//
//  Media.h
//  OneDriveUI
//
//  Created by Vikash Jindal on 27/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "ODDataController.h"
#import "MediaAttribute.h"
@interface Media : NSObject
@property (nonatomic) ID media_id;
@property (nonatomic) int account_id;
@property (nonatomic, strong) NSString * name;
@property (nonatomic, strong) NSString * full_path;
@property (nonatomic, strong) NSString * mime_type;
@property (nonatomic) bool is_folder;
@property (nonatomic) unsigned long long size;
@property (nonatomic, strong) NSString *  icon;
@property (nonatomic, strong) MediaAttribute * attrib;
@property (nonatomic) DateTime last_mod_date;
- (id)initWithValue:(ODMedia) media;
- (void) getODMedia: (ODMedia &)mediaObj;
-(NSString *) getMediaName;
@end
