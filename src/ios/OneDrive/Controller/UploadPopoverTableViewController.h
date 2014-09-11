//
//  UploadPopoverTableViewController.h
//  Test
//
//  Created by labuser on 3/7/13.
//  Copyright (c) 2013 labuser. All rights reserved.
//

#import <UIKit/UIKit.h>
#pragma mark UploadPopoverTableViewControllerDelegate protocol
// Delegate to listen to the PERTemplate selection made
@protocol UploadPopoverTableViewControllerDelegate <NSObject>
@optional
- (void) openGallery;
- (void) openCamera;
- (void) openAddFolderWorkflow;
- (void) copyFromClipboard;
@end
@interface UploadPopoverTableViewController : UITableViewController <UINavigationControllerDelegate, UIImagePickerControllerDelegate>
{
    
    NSUInteger	_tableHeight;
	NSMutableArray*	_sectionsArray;
}

@property (nonatomic, assign) id<UploadPopoverTableViewControllerDelegate> delegate;
@property (nonatomic, retain) NSMutableArray    *sectionsArray;
@property (nonatomic, retain) NSString          *folderName;

- (void) setUpSectionsArray;

@end



@interface UploadMediaStatusController : UITableViewController<UINavigationBarDelegate>
{
     NSUInteger	_tableHeight;
}
@property (nonatomic, retain) NSMutableArray    *sectionsArray;
@end

@interface UploadStatusInfo : NSObject

@property (nonatomic, strong) NSString *localPath;
@property (nonatomic, strong) NSString *name;
@property (nonatomic, strong) NSNumber *totalSize;
@property (nonatomic, strong) NSNumber *sizeCompleted;

@end


@interface UploadStatusCell : UITableViewCell

@property (strong, nonatomic) IBOutlet UIImageView *imgView;
@property (strong, nonatomic) IBOutlet UILabel *name;
@property (strong, nonatomic) IBOutlet UIActivityIndicatorView *prgressView;

@end