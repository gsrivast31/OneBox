//
//  AddFolderViewController.m
//  OneDriveUI
//
//  Created by Gaurav Srivastava on 3/8/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#import "AddFolderViewController.h"

@interface AddFolderViewController ()

@end

@implementation AddFolderViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)createClicked:(id)sender {
    NSString* folderName = self.folderTextEdit.text;
    if ([folderName length] != 0) {
        if ([_delegate respondsToSelector:@selector(createFolder:)]) {
            [_delegate createFolder:folderName];
        }
    }
}

- (IBAction)cancelClicked:(id)sender {
    if ([_delegate respondsToSelector:@selector(removePopovers)]) {
        [_delegate removePopovers];
    }
}
@end
