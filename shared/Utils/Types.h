//
//  Header.h
//  OneDrive
//
//  Created by Ajay on 12/17/12.
//  Copyright (c) 2012 Labs. All rights reserved.
//

#ifndef TYPES_h
#define TYPES_h

#include <vector>
#include <time.h>
#include "UString.h"

namespace NewGroup
{
    namespace Core
    {
        // Make it platform specific
        typedef long long ID;
        typedef std::vector<unsigned char> ThumbData;
        typedef std::vector<ID> ODIdList;
        typedef ODIdList ODMediaIdList;
    
        // This will grow further as we implement new functionalities at the App Controller level.
        enum RESULT
        {
            RESULT_NoErr,
            RESULT_AccountCreationFailed,
            RESULT_AccountAuthenticationFailed,
            RESULT_AccountRetreivalFailed,
            RESULT_DBUpdateFailed,
            RESULT_AccountExists,
            RESULT_MediaCreationFailed,
            RESULT_MediaDeletionFailed,
            RESULT_ThumbnailCreationFailed,
            RESULT_ThumbnailDeletionFailed,
            RESULT_NoMediaChanges
        };
        
        enum MediaType
        {
            eModelMediaImage,
            eModelMediaVideo,
            eModelMediaDocument,
            eModelMediaPDF,
            eModelMediaContact,
            eModelMediaFolder,
            eModelMediaUnknown
        };
        
        struct Date
        {
            unsigned char day;
            unsigned char month;
            unsigned short year;
            Date()
            {
                day = month = 1;
                year = 2000;
            }
        };
        
        struct Time
        {
            unsigned char hour, min, sec;
            unsigned short milliseconds;
            Time()
            {
                hour = min = sec = milliseconds = 0;
            }
        };
        
        class DateTime
        {
            Date datePart;
            Time timePart;
            time_t utcTimestamp;
            
        public:
            DateTime()
            {
                utcTimestamp = 0;
            }
            
            DateTime(const UString& dateString, const UString& format)
            {
                struct tm timeInfo;
                if(!strptime(dateString.getUTF8String().c_str(), format.getUTF8String().c_str(), &timeInfo))
                {
                    throw std::runtime_error("Invalid date time format!");
                }
                else
                {
                    auto localTime = mktime(&timeInfo);
                    
                    *this = DateTime(localTime);
                }
            }
            
            DateTime(time_t timeStamp)
            {
                struct tm timeInfo;
                gmtime_r(&timeStamp, &timeInfo);
                
                datePart.day = timeInfo.tm_mday;
                datePart.month = timeInfo.tm_mon + 1;
                datePart.year = 1900 + timeInfo.tm_year;
                
                timePart.hour = timeInfo.tm_hour;
                timePart.min = timeInfo.tm_min;
                timePart.sec = timeInfo.tm_sec;
                timePart.milliseconds = 0;
                
                utcTimestamp = timeStamp;
            }
            
            time_t GetUTCTime() const
            {
                return utcTimestamp;
            }
            
            Date GetDate() const
            {
                return datePart;
            }
            
            Time GetTime() const
            {
                return timePart;
            }
            
            UString ToString(bool local = true) const
            {
                char buff[255];
                struct tm timeInfo;
                if(local)
                    localtime_r(&utcTimestamp, &timeInfo);
                else
                    gmtime_r(&utcTimestamp, &timeInfo);
                asctime_r(&timeInfo, buff);
                return UString::fromUtf8(buff);
            }
            
        };
    }
}



#endif
