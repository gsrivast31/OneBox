//
//  ODCollectionViewFlowLayout.m
//  OneDriveUI
//
//  Created by Vikash Jindal on 26/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "ODCollectionViewFlowLayout.h"
#import "ODCollectionViewLayoutAttributes.h"
@implementation ODCollectionViewFlowLayout
- (void)prepareLayout
{
    [super prepareLayout];
    int width = 120;
    int height = 134;
    if ([[self.collectionView.delegate class] conformsToProtocol:@protocol(ODLayoutDelegate)])
    {
        
        width = [(id)self.collectionView.delegate getRowWidth: self.collectionView layout:self];
        if(width != 120)
            height = 50;
        // ToDo: (abedi) Query for height from the delegate.. Don't hardcode this. A lot of cleanup like this is pending. This really makes tough to debug the UI problems.
        
    }
    self.itemSize = CGSizeMake(width, height);
    self.minimumInteritemSpacing = 0;
    self.minimumLineSpacing = 0;
    self.scrollDirection = UICollectionViewScrollDirectionVertical;
    self.sectionInset = UIEdgeInsetsMake(0, 0, 0, 0);
 
}

- (BOOL)isDeletionModeOn
{
    if ([[self.collectionView.delegate class] conformsToProtocol:@protocol(ODLayoutDelegate)])
    {
        return [(id)self.collectionView.delegate isDeletionModeActiveForCollectionView:self.collectionView layout:self];
    }
    return NO;
}

+ (Class)layoutAttributesClass
{
    return [ODCollectionViewLayoutAttributes class];
}

- (ODCollectionViewLayoutAttributes *)layoutAttributesForItemAtIndexPath:(NSIndexPath *)indexPath
{
    ODCollectionViewLayoutAttributes *attributes = (ODCollectionViewLayoutAttributes *)[super layoutAttributesForItemAtIndexPath:indexPath];
    if ([self isDeletionModeOn])
        attributes.deleteButtonHidden = NO;
    else
        attributes.deleteButtonHidden = YES;
    return attributes;
}

- (NSArray *)layoutAttributesForElementsInRect:(CGRect)rect
{
    NSArray *attributesArrayInRect = [super layoutAttributesForElementsInRect:rect];
    for (ODCollectionViewLayoutAttributes *attribs in attributesArrayInRect)
    {
        if ([self isDeletionModeOn]) attribs.deleteButtonHidden = NO;
        else attribs.deleteButtonHidden = YES;
    }
    return attributesArrayInRect;
}

@end
