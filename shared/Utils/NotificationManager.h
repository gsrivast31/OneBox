//
//  NotificationManager.h
//  emptyapp
//
//  Created by Ruchin Kansal on 13/03/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef emptyapp_Notifications_h
#define emptyapp_Notifications_h
#include <functional>
#include <memory>
#include <list>
#include <mutex>
#include <thread>
namespace NewGroup
{
    namespace Core
    {
        template <typename... NotificationDataT>
        class NotificationManager
        {
        public:
            typedef std::function<void (NotificationDataT...)> ObserverType_t;
            typedef std::shared_ptr<ObserverType_t> SharedObserverType_t;
        private:
            typedef std::weak_ptr<ObserverType_t> WeakObserverType_t;
            typedef std::list<WeakObserverType_t> ObserversList_t;
            typedef std::lock_guard<std::recursive_mutex> LockGuard;
            
            ObserversList_t mObservers;
            std::recursive_mutex mLock;
            
        protected:
            
            void Dispatch(NotificationDataT... notificationData)
            {
                LockGuard lock(mLock);
                auto observer = mObservers.begin();
                while (observer != mObservers.end())
                {
                    auto sharedObserverPtr = observer->lock();
                    if(sharedObserverPtr)
                    {
                        (*sharedObserverPtr)(notificationData...);
                        ++observer;
                    }
                    else
                    {
                        //Delete the observer if it is not there now!
                        observer = mObservers.erase(observer);
                    }
                }
            }
            
        public:
            typedef typename ObserversList_t::const_iterator ObserverHandle_t;
            
            SharedObserverType_t MakeShared(ObserverType_t observer)
            {
                return std::make_shared<ObserverType_t>(observer);
            }
            ObserverHandle_t operator += (WeakObserverType_t observer)
            {
                LockGuard lock(mLock);
                mObservers.push_back(observer);
                auto endit = mObservers.cend();
                return --endit;
            }
            
            void operator -= (WeakObserverType_t handle)
            {
                LockGuard lock(mLock);
                mObservers.erase(handle);
            }
            
            ObserverHandle_t AddObserver(WeakObserverType_t observer)
            {
                return *this += observer;
            }
            
            void RemoveObserver(WeakObserverType_t handle)
            {
                *this -= handle;
            }
        };

        
    }
}


#endif

