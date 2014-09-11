//
//  FileInfoController.h
//  OneDrive
//
//  Created by Gaurav Srivastava on 4/16/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppController.h"

@interface FileInfoController : UITableViewController
{
    NSMutableArray*	_rowsArray;
    ODMedia media;
}

@property (nonatomic, retain) NSMutableArray *rowsArray;

- (void) setUpRowsArray;
- (void) setMediaNode:(ODMedia)media;

@end
