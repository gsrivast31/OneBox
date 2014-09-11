//
//  CommonDefines.h
//  webservices_curl
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef webservices_curl_CommonDefines_h
#define webservices_curl_CommonDefines_h
#include "PlatformConfig.h"

#if BUILD_PLATFORM_MACOS
    #include <AppKit/NSView.h>
    #import <Foundation/Foundation.h>
    #import <AVFoundation/AVFoundation.h>
#elif BUILD_PLATFORM_IOS
    #include "UIKit/UIView.h"
    #import <Foundation/Foundation.h>
    #import <AVFoundation/AVFoundation.h>
#endif

namespace NewGroup
{
    namespace Core
    {
#ifdef BUILD_PLATFORM_WINDOWS
        typedef HWND PlatformView;
#elif BUILD_PLATFORM_IOS
        typedef UIView* PlatformView;
#elif BUILD_PLATFORM_MACOS
        typedef NSView* PlatformView;
#endif

        enum HTTPReturnCodes
        {
            kHTTPResponseOK = 200,
            kHTTPResponseBlank = 204,
            kHTTPTokenExpired = 401,
            
            kHTTPUnknownError
        };
        
        enum AuthenticationResult
        {
            kAuthenticationPassed,
            kAuthenticationDenied,
            kAuthenticationCancelled,
            kAuthenticationUnknownErr
        };
        
        enum TaskPriority
        {
            kTaskPriority_High = 0,
            kTaskPriority_Normal,
            kTaskPriority_Low
        };
    }
}

#endif
