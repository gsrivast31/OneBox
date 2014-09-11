//
//  Timer.cpp
//  AccountServices
//
//  Created by Ruchin Kansal on 20/06/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#include "Timer.h"

#ifdef BUILD_PLATFORM_IOS
@implementation TimerCallbackWrapper
-(void) setCallback:(NewGroup::Core::Utils::WeakTimerCallBackProc_t)cb{
    callback = cb;
}
-(NewGroup::Core::Utils::WeakTimerCallBackProc_t) getCallback{
    return callback;
}

-(void) timerCallback{
    auto sharedTimerProc = callback.lock();
    if(sharedTimerProc)
    {
        (*sharedTimerProc)();
    }
}

@end
#endif