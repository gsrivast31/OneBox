#pragma once
#ifdef _WIN64
	#define BUILD_PLATFORM_WIN64 (1)
	#define BUILD_PLATFORM_WINDOWS (1)
#elif _WIN32
	#define BUILD_PLATFORM_WINDOWS (1)
	#define BUILD_PLATFORM_WIN32 (1)
#elif __APPLE__
    #include "TargetConditionals.h"
	#define BUILD_PLATFORM_APPLE (1)
    #if TARGET_OS_IPHONE    
         #define BUILD_PLATFORM_IOS (1)
         #define BUILD_PLATFORM_IOS_DEVICE (1)
    #elif TARGET_IPHONE_SIMULATOR
        #define BUILD_PLATFORM_IOS (1)
        #define BUILD_PLATFORM_IOS_SIMULATOR (1)
    #elif TARGET_OS_MAC
        #define BUILD_PLATFORM_MACOS (1)
    #else
		#error Unsupported Apple platform
    #endif
#else
	#error Unsupported Platform
#endif

#ifdef BUILD_PLATFORM_WINDOWS
#include <Windows.h>
#endif


#ifdef __APPLE__
#define Macintosh 1
#endif