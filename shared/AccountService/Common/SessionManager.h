//
//  SessionManager.h
//  AccountServices
//
//  Created by Ruchin Kansal on 20/06/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef AccountServices_SessionManager_h
#define AccountServices_SessionManager_h

#include "Timer.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include "mutex"

namespace NewGroup {
    namespace Core{
        using namespace Utils;
        typedef std::function<void()> SessionCallBackProc_t;
        typedef NewGroup::Core::Utils::TypeFactory<SessionCallBackProc_t>::SharedPtr SharedSessionCallBackProc_t;
        typedef NewGroup::Core::Utils::TypeFactory<SessionCallBackProc_t>::WeakPtr WeakSessionCallBackProc_t;
        
        typedef std::function<void(const UString& accessToken, time_t expiresIn, int error)> RefreshTokenCallbackProc_t;
        
        typedef std::function<void(RefreshTokenCallbackProc_t)> RefreshTokenProc_t;
        
        typedef std::function<void(const UString& accessToken)> AccessTokenCallbackProc_t;
        
        class SessionManager
        {
        public:
            
            SessionManager(const UString& accessToken, std::time_t expiresIn, RefreshTokenProc_t refreshProc);
            
            bool isTokenValid() const;
            
            void GetAccessToken(AccessTokenCallbackProc_t callback);
            
        private:
            UString mAccessToken;
            std::time_t mExpiryTime;
            bool mIsRequestPending;
            std::vector<AccessTokenCallbackProc_t> mCallBacks;
            std::recursive_mutex mMutex;
            
            void ManageToken(int expiryTime, WeakSessionCallBackProc_t callback)
            {
                mMainThreadCallback.reset(new SessionCallBackProc_t(std::bind(&SessionManager::InstallTimer, this, expiryTime, callback)));
                ExecuteOnMainThread(mMainThreadCallback);
            }
            
            void InstallTimer(int expiryTime, WeakSessionCallBackProc_t callback)
            {
                mTimerCallback.reset(new TimerCallBackProc_t(std::bind(&SessionManager::TimeoutFunc, this, callback)));
                ExecuteOnTimer(mTimerCallback, expiryTime);
            }
            
            void TimeoutFunc(WeakSessionCallBackProc_t clientCallback)
            {
                auto sharedCallback = clientCallback.lock();
                if(sharedCallback)
                    (*sharedCallback)();
            }
                                     
            SharedTimerCallBackProc_t mTimerCallback, mMainThreadCallback;
            RefreshTokenProc_t mRefreshTokenProc;
            std::time_t GetCurrentTime() const;
        };
    }
}

#endif
