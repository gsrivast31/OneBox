//
//  IconCell.h
//  OneDriveUI
//
//  Created by Vikash Jindal on 22/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FileUploadManager.h"

@interface UploadProgressView : UIView
@property (strong, nonatomic) IBOutlet UIActivityIndicatorView *activityView;
@property (strong, nonatomic) IBOutlet UILabel *progressLabel;

@end

@interface UploadProgressViewController : UIViewController
{
    float totalSize; // In Bytes
    float uploadedSize; // Currently uploaded Bytes
}

-(void) startProgress: (float)finished totalSize:(float)tSize;
-(void) updateProgress: (float)finished totalSize:(float)tSize;
-(void) userCancelled; // For now keeping this, but eventually we would need to implement this.
-(void) stopProgress;

@property (nonatomic, strong) UploadProgressView *mProgressView;
@property (nonatomic, strong) UIActivityIndicatorView *mActivityView;
@end



@interface IconCell : UICollectionViewCell
{
    UString mediaPath;// mediaPath for this cell.
    FileUploadProgressManager::UploadFileProgressManager_t::SharedObserverType_t uploadEventHandler;
}

-(void) showIndicator;
- (UString) getMediaPath;

@property (nonatomic, weak) IBOutlet UILabel *label;
@property (nonatomic, weak) IBOutlet UIButton *deleteButton;
@property (weak, nonatomic) IBOutlet UIImageView *thumbnail;
@property (nonatomic, strong) UploadProgressViewController *pController;

           
@end


@interface listViewCell : UICollectionViewCell

@property (nonatomic, weak) IBOutlet UILabel *name;
@property (nonatomic, weak) IBOutlet UILabel *date;
@property (nonatomic, weak) IBOutlet UILabel *size;
@property (weak, nonatomic) IBOutlet UIImageView *thumbnail;


@end