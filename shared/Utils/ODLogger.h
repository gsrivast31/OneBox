//
//  ODLogger.h
//  TestWebServices
//
//  Created by Ruchin Kansal on 18/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef TestWebServices_ODLogger_h
#define TestWebServices_ODLogger_h
#include "PlatformConfig.h"
#include "UString.h"
#include <sstream>
#include <map>
#include <ctime>
#include <chrono>
#include <fstream>
#include <mutex>

namespace NewGroup
{
    namespace Core
    {
        class Logger 
        {
            Logger(){}
            
            ~Logger()
            {
            }
            
            UTF8String GetCurrentDateTime()
            {
                UTF8String strTime;
                strTime.resize(256);
                auto currTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                auto localTime = std::localtime(&currTime);
                std::strftime((char *)strTime.data(), strTime.capacity(), "[%d-%m-%y %H:%M:%S]", localTime);
                
                return strTime;
            }
            
            size_t mMaxLogFileSize;
            UTF8String mFileName;
            std::fstream mLogStream;
            std::mutex m;
        public:
            static Logger& GetInstance()
            {
                static Logger *pLogger = new Logger;
                return *pLogger;
            }
            
            void InitializeLogger(const char *strLogFilePath)
            {
                mMaxLogFileSize = 20*1024*1024; //20 MB
                mFileName = strLogFilePath;
                mLogStream.open(strLogFilePath, std::ios::out);
                
                WriteMessage("Application Started!!");
            }
            
            void WriteMessage(const UString &msg)
            {
                std::lock_guard<std::mutex> lock(m);
                try
                {
                    if (mLogStream.good())
                    {
                        if(mLogStream.tellg() >= mMaxLogFileSize)
                        {
                            //Rollver the file now!
                            mLogStream.close();
                            mLogStream.open(mFileName, std::ios::out);
                            
                            if (mLogStream.good())
                            {
                                mLogStream << GetCurrentDateTime() << ": Log file Flushed since its size exceeded " << mMaxLogFileSize << "bytes." << std::endl;
                            }
                            else
                                return;
                        }
                        
                        auto strLogMsg = GetCurrentDateTime() + UTF8String(": ") + msg.getUTF8String();                        
                        mLogStream << strLogMsg << std::endl;
                        mLogStream.flush();

                        LogOnPlatform(msg.getUTF8String().c_str());
                        
                        if (!mLogStream.good())
                        {
                            mLogStream.clear();
                        }
                    }
                }
                catch(...)
                {
                    LogOnPlatform("Exception while writing message in log file.");
                }
            }
            
            void LogOnPlatform(const char *msg)
            {
#ifdef BUILD_PLATFORM_APPLE
                NSLog(@"%s", msg);
#endif
            }
        };
        
        namespace Utils
        {
            inline void PlatformLog(const char *msg)
            {
                Logger::GetInstance().LogOnPlatform(msg);
            }
            
            inline UTF8String GetApplicationLogPath(const char *strLogFileName)
            {
                UTF8String logFilePath;
#ifdef BUILD_PLATFORM_IOS
                NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
                NSString *documentsDirectory = [paths objectAtIndex:0];
                NSString *filePath = [documentsDirectory stringByAppendingPathComponent:[NSString stringWithUTF8String:strLogFileName]];
                logFilePath = [filePath UTF8String];
                
#endif
                PlatformLog((UTF8String("Log file Path: ") + logFilePath).c_str());
                return logFilePath;
            }
            
            inline void InitializeLogger(const char *strLogFileName)
            {
                Logger::GetInstance().InitializeLogger(GetApplicationLogPath(strLogFileName).c_str());
            }
            
            inline void Log(const UString &msg)
            {
                Logger::GetInstance().WriteMessage(msg);
            }
            
            
        }
    }
}

#endif
