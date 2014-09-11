//
//  PopupBase.h
//  OneDriveUI
//
//  Created by Gaurav Srivastava on 3/12/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PopupBase : NSObject
{
    NSInteger   _index;
    NSString*	_name;
    NSString*	_image;
}

@property (nonatomic, retain) NSString    *name;
@property (nonatomic, retain) NSString    *image;
@property (nonatomic) NSInteger index;
@end
