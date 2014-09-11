//
//  ShareServiceInterface.h
//  ShareServices
//
//  Created by Ruchin Kansal on 17/01/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef ShareServices_ShareServiceInterface_h
#define ShareServices_ShareServiceInterface_h
#include "UString.h"
#include "Exception.h"
#include "ODAssert.h"
#include "Types.h"
#include <map>
#include <functional>
namespace NewGroup
{
    namespace Core
    {
        enum ShareServicesType
        {
            kShareViaFacebook = 1,
            kShareViaTwitter = kShareViaFacebook << 1,
            kShareViaEmail = kShareViaTwitter << 1
        };
        
        enum ShareServiceResult
        {
            kNoError = 0,
            //Define additional codes here
            
            kAuthenticationError,
            kFileFormatNotSupported,
            kGenericError
        };
        
        class ShareServiceInterface:public std::enable_shared_from_this<ShareServiceInterface>
        {
        public:
            typedef std::shared_ptr<ShareServiceInterface> SharedServicePtr;
            typedef std::function<void (ShareServiceResult, const UString &errorString)> ShareCallBackProc_t;
            
            virtual void ShareURL(const UString &url, const UString& desc, ShareCallBackProc_t callback) = 0;
            virtual ~ShareServiceInterface(){}
            
            friend SharedServicePtr CreateShareServiceInstance(int);
        };
        
        ShareServiceInterface::SharedServicePtr CreateShareServiceInstance(int);
    }
}
#endif
