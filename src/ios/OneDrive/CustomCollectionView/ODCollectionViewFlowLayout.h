//
//  ODCollectionViewFlowLayout.h
//  OneDriveUI
//
//  Created by Vikash Jindal on 26/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
@protocol ODLayoutDelegate
@required
- (BOOL)isDeletionModeActiveForCollectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout;
- (NSInteger) getRowWidth:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout;
@end
@interface ODCollectionViewFlowLayout : UICollectionViewFlowLayout

@end
