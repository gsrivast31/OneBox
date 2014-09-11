//
//  Media.m
//  OneDriveUI
//
//  Created by Vikash Jindal on 27/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "Media.h"

@implementation Media
- (id)initWithValue:(ODMedia) media{
	self = [super init];
	if (self) {
		_media_id = media.media_id;
        _account_id = media.account_id;
        _name = media.file_id.getPlatformString();
        _full_path = media.full_path.getPlatformString();
        _size = media.size;
        _icon = media.icon.getPlatformString();
        //init part for name && full_path;
        _mime_type = media.mime_type.getPlatformString();
        _is_folder = media.is_folder;
        _last_mod_date = media.mod_date;
	}
	
	return self;
}

- (void) getODMedia: (ODMedia &)mediaObj
{
 
    mediaObj.media_id = _media_id;
    mediaObj.account_id = _account_id;
    mediaObj.file_id =  _name;
    mediaObj.full_path = _full_path ;
    mediaObj.size = _size;
     mediaObj.icon = _icon;
     mediaObj.mime_type = _mime_type;
    mediaObj.is_folder = _is_folder;
    mediaObj.mod_date =  _last_mod_date;

}

-(NSString *) getMediaName{
    NSArray *pathArray = [_full_path pathComponents];
    return [pathArray lastObject];
}
@end
