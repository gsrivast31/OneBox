//
//  SocialImpl.h
//  ShareServices
//
//  Created by Ruchin Kansal on 20/03/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef ShareServices_SocialImpl_h
#define ShareServices_SocialImpl_h

#include <iostream>
#include "ShareServiceInterface.h"

namespace NewGroup
{
    namespace Core
    {
        class SocialImpl: public ShareServiceInterface
        {
            ShareServicesType mServiceType;
        protected:
            SocialImpl(ShareServicesType type):mServiceType(type){}
            
            void ShareURL(const UString &url, const UString& desc, ShareCallBackProc_t callback);
            
        };
    }
}

#endif
