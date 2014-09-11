//
//  NotificationManagerImpl.h
//  AccountServices
//
//  Created by Ruchin Kansal on 14/03/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef AccountServices_NotificationManagerImpl_h
#define AccountServices_NotificationManagerImpl_h

#include "NotificationManager.h"
namespace NewGroup
{
    namespace Core
    {
        template <typename NotificationDataT>
        class NotificationManagerImpl:public NotificationManager<NotificationDataT>
        {
        public:
            void Dispatch(NotificationDataT notificationData)
            {
                NotificationManager<NotificationDataT>::Dispatch(notificationData);
            }
        };
        
    }
}


#endif
