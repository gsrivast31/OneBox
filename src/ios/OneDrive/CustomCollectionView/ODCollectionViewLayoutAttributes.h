//
//  ODCollectionViewLayoutAttributes.h
//  OneDriveUI
//
//  Created by Vikash Jindal on 26/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ODCollectionViewLayoutAttributes : UICollectionViewLayoutAttributes
@property (nonatomic, getter = isDeleteButtonHidden) BOOL deleteButtonHidden;
@end