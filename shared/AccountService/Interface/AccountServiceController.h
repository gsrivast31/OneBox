//
//  AccountServiceController.h
//  AccountServices
//
//  Created by Ruchin Kansal on 17/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef AccountServices_AccountServiceController_h
#define AccountServices_AccountServiceController_h

#include "AccountServiceInterface.h"
#include "Types.h"
#include <map>


namespace NewGroup
{
    namespace Core
    {
        class AccountServiceController: public std::enable_shared_from_this<AccountServiceController>
        {
            typedef std::function<void (AuthenticationResult, const UString &authToken)> AuthCallBackProc_t;
            
            AccountServiceController();
            friend AccountServiceInterface::ASEngineSharedPtr CreateServiceInstance(ServiceProvider provider);
        public:
            typedef std::shared_ptr<AccountServiceController> ASControllerSharedPtr;
            typedef std::vector<ServiceInfo> AccountServicesList;
            
            static ASControllerSharedPtr GetInstance();
            ~AccountServiceController();
            AccountServiceInterface::ASEngineSharedPtr CreateServiceInstance(ServiceProvider provider);
            void GetServiceInfo(ServiceProvider provider, ServiceInfo &info);
            void ListValidServices(AccountServicesList &validServices);
            
            bool AddAccountForService(ServiceProvider service, AuthCallBackProc_t callback);
            
        private:
        };
    }
}
#endif
