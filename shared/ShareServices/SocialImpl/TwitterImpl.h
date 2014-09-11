//
//  TwitterImpl.h
//  ShareServices
//
//  Created by Ruchin Kansal on 20/03/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef ShareServices_TwitterImpl_h
#define ShareServices_TwitterImpl_h

#include "SocialImpl.h"
namespace NewGroup
{
    namespace Core
    {
        class TwitterServiceImpl: public SocialImpl
        {
            
            TwitterServiceImpl();
            friend SharedServicePtr CreateShareServiceInstance(int);
            
        public:
            ~TwitterServiceImpl();
        };
    }
}


#endif
