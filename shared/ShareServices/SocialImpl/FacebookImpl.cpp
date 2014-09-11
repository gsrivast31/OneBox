//
//  FacebookImpl.cpp
//  ShareServices
//
//  Created by Ruchin Kansal on 18/01/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#include "FacebookImpl.h"

#import <FacebookSDK.h>
#include "StringConstants.h"
#import "Social/SLComposeViewController.h"
#import "Social/SLServiceTypes.h"

using namespace NewGroup::Core;
using namespace NewGroup::Core::Facebook;

FacebookServiceImpl::FacebookServiceImpl():SocialImpl(kShareViaFacebook)
{
}

FacebookServiceImpl::~FacebookServiceImpl()
{
    
}
