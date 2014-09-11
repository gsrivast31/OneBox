//
//  ViewController.m
//  TestWebServices
//
//  Created by Ruchin Kansal on 10/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#import "ViewController.h"
#include "CommonUtils.h"
#include "AccountServiceController.h"
#include "AppController.h"
#include "ODDataController.h"
#include "ODLogger.h"

using namespace NewGroup::Core;


void testcallback(RESULT, ODAccount )
{

}

@interface ViewController ()

@end

@implementation ViewController




- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    AppController &applicationCtr = AppController::instance();
    
    typedef std::function<void (RESULT, ODAccount)> callback_addAccount ;
    
    applicationCtr.SetParentView(self.view);
    
    applicationCtr.AddAccountForService(kSPDropbox, "test", testcallback);
   /*
    auto dropBoxInstance = AccountServiceController::GetInstance()->CreateServiceInstance(kSPDropbox);
    dropBoxInstance->Authenticate(self.view, [dropBoxInstance](AuthenticationResult result, const UString &token){
        Utils::Log(token);
    });
    */
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
