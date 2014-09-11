//
//  ShareServices.m
//  ShareServices
//
//  Created by Ruchin Kansal on 17/01/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#import "ShareServices.h"
#include "FacebookImpl.h"
#include "TwitterImpl.h"

@implementation ShareServices

@end
namespace NewGroup
{
    namespace Core
    {
        ShareServiceInterface::SharedServicePtr CreateShareServiceInstance(int type)
        {
            static ShareServiceInterface::SharedServicePtr facebookService, twitterService;
            ShareServiceInterface::SharedServicePtr retValue;
            switch(type)
            {
                case kShareViaFacebook:
                    if(!facebookService)
                        facebookService.reset(new FacebookServiceImpl);
                    retValue = facebookService;
                    break;
                case kShareViaTwitter:
                    if(!twitterService)
                        twitterService.reset(new TwitterServiceImpl);
                    retValue = twitterService;
                    break;
            }
            
            return retValue;
        }
    }
}
