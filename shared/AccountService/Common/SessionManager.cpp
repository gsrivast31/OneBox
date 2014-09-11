//
//  SessionManager.cpp
//  AccountServices
//
//  Created by Ruchin Kansal on 21/06/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#include "SessionManager.h"
using namespace NewGroup::Core;

SessionManager::SessionManager(const UString& accessToken, std::time_t expiresIn, RefreshTokenProc_t refreshProc):mAccessToken(accessToken), mRefreshTokenProc(refreshProc)
{
    mExpiryTime = GetCurrentTime() + expiresIn;
    mIsRequestPending = false;
}

std::time_t SessionManager::GetCurrentTime() const
{
    using std::chrono::system_clock;
    return system_clock::to_time_t(system_clock::now());
}

bool SessionManager::isTokenValid() const
{
    std::time_t delta = 60; // 60 seconds off
    auto timeNow = GetCurrentTime();
    
    return timeNow < mExpiryTime && (mExpiryTime - timeNow) > delta;
}

void SessionManager::GetAccessToken(AccessTokenCallbackProc_t callback)
{
    std::unique_lock<std::recursive_mutex> lock(mMutex);
    
    if(isTokenValid())
    {
        callback(mAccessToken);
    }
    else{
        mCallBacks.push_back(callback);
        if(!mIsRequestPending)
        {
            mRefreshTokenProc([this](const UString& accessToken, time_t expiresIn, int error){
                std::unique_lock<std::recursive_mutex> lock(mMutex);
                mIsRequestPending = false;
                mExpiryTime = GetCurrentTime() + expiresIn;
                mAccessToken = accessToken;
                
                for(auto callback:mCallBacks)
                {
                    callback(mAccessToken);
                }
            });
        }
    }
}