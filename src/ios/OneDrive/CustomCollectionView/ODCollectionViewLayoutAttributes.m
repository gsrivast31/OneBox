//
//  ODCollectionViewLayoutAttributes.m
//  OneDriveUI
//
//  Created by Vikash Jindal on 26/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "ODCollectionViewLayoutAttributes.h"

@implementation ODCollectionViewLayoutAttributes
- (id)copyWithZone:(NSZone *)zone
{
    ODCollectionViewLayoutAttributes *attributes = [super copyWithZone:zone];
    attributes.deleteButtonHidden = _deleteButtonHidden;
    return attributes;
}
@end
