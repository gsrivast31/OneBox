//
//  AccountServiceController.cpp
//  AccountServices
//
//  Created by Ruchin Kansal on 17/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#include "AccountServiceController.h"
#include "DropboxImpl.h"
#include "GdriveImpl.h"
#include "SkyDriveImpl.h"

using namespace NewGroup::Core;

AccountServiceController::AccountServiceController()
{
    
}

AccountServiceController::~AccountServiceController()
{

}

AccountServiceController::ASControllerSharedPtr AccountServiceController::GetInstance()
{
    static AccountServiceController::ASControllerSharedPtr singleController(new AccountServiceController);
    return singleController;
}

AccountServiceInterface::ASEngineSharedPtr AccountServiceController::CreateServiceInstance(ServiceProvider provider)
{
    AccountServiceInterface::ASEngineSharedPtr engineInstance;
    switch(provider)
    {
        case kSPDropbox:
        {
            AccountServiceInterface *pInstance = new DropboxImpl;
            engineInstance.reset(pInstance);
        }
        break;
        case kSPGDrive:
        {
            AccountServiceInterface *pInstance = new GdriveImpl;
            engineInstance.reset(pInstance);
        }
            break;
        case kSPSkyDrive:
        {
            AccountServiceInterface *pInstance = new SkyDriveImpl;
            engineInstance.reset(pInstance);
        }
            break;
        default:
        {
            
        }
        break;
    }
    
    return engineInstance;
}

bool AccountServiceController::AddAccountForService(ServiceProvider service, AuthCallBackProc_t callback)
{
    bool result = false;
    return result;
}

void AccountServiceController::GetServiceInfo(ServiceProvider provider, ServiceInfo &info)
{
    switch(provider)
    {
        case kSPDropbox:
        {
            info.mServiceName = UString(L"Dropbox");
            info.mServiceProvider = provider;
            info.mServiceType = kAccountTypeOnlineSharing;
            info.mIconPath = UString(L"");
        }
        break;
            
        default:
        {
            
        }
        break;
    }

}

void AccountServiceController::ListValidServices(AccountServicesList &validServices)
{
    validServices.clear();
    validServices.resize(1);
    GetServiceInfo(kSPDropbox, validServices[0]);
}