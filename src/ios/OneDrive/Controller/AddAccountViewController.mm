//
//  AddAccountViewController.m
//  OneDrive
//
//  Created by Vikash Jindal on 19/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "AddAccountViewController.h"
#include "CommonUtils.h"
#include "AccountServiceController.h"
#include "ODDataController.h"
#include "ODLogger.h"
#import "WrappedAccountData.h"
using namespace NewGroup::Core;
//void CallBackAccountForMedia(RESULT res, ODMedia media, bool deleted, bool fromCache, id callerObject);
//{
//    NSLog(@"CallBackAccountForMedia1 call back received");
//
//}
void AddAccountResponseCallBack(RESULT res, ODAccount account)
{
    
    NSNotification *notification = nil;
    if (res == RESULT_NoErr)
    {
        WrappedAccountData *data = [[WrappedAccountData alloc] init];
        data.account = account;
        notification = [NSNotification notificationWithName:@"AccountAddResponse" object:data];
    }
    else{
      notification = [NSNotification notificationWithName:@"AccountAddResponse" object:nil];
    }
    [[NSNotificationCenter defaultCenter] postNotification:notification];
}
@interface AddAccountViewController ()
-(void) AddAccountForService;

@end

@implementation AddAccountViewController

#pragma mark -
#pragma mark SubstitutableDetailViewController

// -------------------------------------------------------------------------------
//	setNavigationPaneBarButtonItem:
//  Custom implementation for the navigationPaneBarButtonItem setter.
//  In addition to updating the _navigationPaneBarButtonItem ivar, it
//  reconfigures the toolbar to either show or hide the
//  navigationPaneBarButtonItem.
// -------------------------------------------------------------------------------
- (void)setNavigationPaneBarButtonItem:(UIBarButtonItem *)navigationPaneBarButtonItem
{
}

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

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(dismiss:) name:@"AccountAddResponse" object:nil];

	// Do any additional setup after loading the view.
    [self AddAccountForService];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
-(void)dealloc
{
}

- (IBAction)close:(id)sender {
    if([sender isKindOfClass:[WrappedAccountData class]])
    {
        NSNotification *notification = [NSNotification notificationWithName:@"UpdateAccountView" object:nil];
        [[NSNotificationCenter defaultCenter] postNotification:notification];

        //if no error post a notification ot fetch the data in rootview controller
        NSNotification *notify = [NSNotification notificationWithName:@"AccountFetched" object:sender];
        [[NSNotificationCenter defaultCenter] postNotification:notify];
    }
    [self.navigationController popToRootViewControllerAnimated: YES];
}

-(void) dismiss:(NSNotification*)notificaion
{
    [self performSelectorOnMainThread:@selector(close:) withObject:[notificaion object] waitUntilDone:YES];
}

-(void) AddAccountForService
{
    AppController &applicationCtr = AppController::instance();
    applicationCtr.SetParentView(self.view);
    applicationCtr.AddAccountForService(_serviceID, _name, AddAccountResponseCallBack);
    
}
@end
