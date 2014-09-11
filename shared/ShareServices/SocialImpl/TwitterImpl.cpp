//
//  TwitterImpl.cpp
//  ShareServices
//
//  Created by Ruchin Kansal on 20/03/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#include "TwitterImpl.h"
#import "Social/SLRequest.h"
#include "StringConstants.h"

using namespace NewGroup::Core;
using namespace NewGroup::Core::Facebook;

TwitterServiceImpl::TwitterServiceImpl():SocialImpl(kShareViaTwitter)
{
    
}

TwitterServiceImpl::~TwitterServiceImpl()
{
    
}

