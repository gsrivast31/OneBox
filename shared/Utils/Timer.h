//
//  Timer.h
//  AccountServices
//
//  Created by Ruchin Kansal on 20/06/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef AccountServices_Timer_h
#define AccountServices_Timer_h
#include <functional>
#include "PlatformConfig.h"
#include "CommonUtils.h"

namespace NewGroup {
    namespace Core{
        namespace Utils{
            typedef std::function<void()> TimerCallBackProc_t;
            typedef NewGroup::Core::Utils::TypeFactory<TimerCallBackProc_t>::SharedPtr SharedTimerCallBackProc_t;
            typedef NewGroup::Core::Utils::TypeFactory<TimerCallBackProc_t>::WeakPtr WeakTimerCallBackProc_t;
        }
    }
}



#ifdef BUILD_PLATFORM_IOS
@interface TimerCallbackWrapper:NSObject{
    NewGroup::Core::Utils::WeakTimerCallBackProc_t callback;
}
-(void) setCallback:(NewGroup::Core::Utils::WeakTimerCallBackProc_t)cb;
-(NewGroup::Core::Utils::WeakTimerCallBackProc_t) getCallback;
-(void) timerCallback;
@end

#endif

namespace NewGroup {
    namespace Core{
        namespace Utils{
            inline void ExecuteOnTimer(WeakTimerCallBackProc_t timerProc, int seconds)
            {
#ifdef BUILD_PLATFORM_IOS
                TimerCallbackWrapper *wrapper = [[TimerCallbackWrapper alloc] init];
                [wrapper setCallback:timerProc];
                [NSTimer scheduledTimerWithTimeInterval:seconds target:wrapper selector:@selector(timerCallback) userInfo:nil repeats:NO];
#endif
            }
            
            inline void ExecuteOnMainThread(WeakTimerCallBackProc_t timerProc)
            {
#ifdef BUILD_PLATFORM_IOS
                TimerCallbackWrapper *wrapper = [[TimerCallbackWrapper alloc] init];
                [wrapper setCallback:timerProc];
                [wrapper performSelectorOnMainThread: @selector(timerCallback) withObject:nil waitUntilDone:YES];
#endif
            }
        }
    }
}
#endif
