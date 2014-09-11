//
//  SearchIconCell.h
//  OneDriveUI
//
//  Created by Ajay Bedi on 22/12/12.
//  Copyright (c) 2013 Ajay Bedi. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SearchIconCell : UICollectionViewCell
{
    IBOutlet UIImageView *mThumbnail;
    IBOutlet UILabel *mAssetName;
    IBOutlet UILabel *mDate;
    IBOutlet UILabel *mSize;
}
@property (weak, nonatomic) IBOutlet UIImageView *mThumbnail;
@property (nonatomic, weak) IBOutlet UILabel *mAssetName;
@property (nonatomic, weak) IBOutlet UILabel *mDate;
@property (nonatomic, weak) IBOutlet UILabel *mSize;

           
@end
