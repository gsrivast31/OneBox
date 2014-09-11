//
//  FacebookImpl.h
//  ShareServices
//
//  Created by Ruchin Kansal on 18/01/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef __ShareServices__FacebookImpl__
#define __ShareServices__FacebookImpl__

#include "SocialImpl.h"
namespace NewGroup
{
    namespace Core
    {
        class FacebookServiceImpl: public SocialImpl
        {
                        
            FacebookServiceImpl();
            
            friend SharedServicePtr CreateShareServiceInstance(int);
            
        public:
            ~FacebookServiceImpl();
        };
    }
}

#endif /* defined(__ShareServices__FacebookImpl__) */
