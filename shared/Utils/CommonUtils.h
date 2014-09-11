//
//  CommonUtils.h
//  webservices_curl
//
//  Created by Ruchin Kansal on 11/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef webservices_curl_CommonUtils_h
#define webservices_curl_CommonUtils_h

#include "UString.h"
#include <sstream>
#include <map>
#include <ctime>
#include <chrono>
#include "ODLogger.h"
#include <time.h>
#include "CommonDefines.h"
#include <set>


namespace NewGroup
{
    namespace Core
    {
        namespace Utils
        {
            template <typename SomeClass>
            struct TypeFactory
            {
                typedef std::shared_ptr<SomeClass> SharedPtr;
                typedef std::weak_ptr<SomeClass> WeakPtr;
            };
            
            template<typename T>
            class RawData_t
            {
            public:
                typedef typename TypeFactory<T>::SharedPtr SharedDataPtr;
            private:
                SharedDataPtr mData;
                size_t mSize;
                
            public:
                RawData_t():mSize(0){
                    
                }
                
                RawData_t<T> Move(T* data, size_t size)
                {
                    mSize = size;
                    mData.reset(data);
                    return *this;
                }
                
                RawData_t<T> Copy(const T* data, size_t size)
                {
                    T* newData = new T[size];
                    memcpy(newData, data, size*sizeof(T));
                    Move(newData, size);
                    
                    return *this;
                }
                
                static RawData_t<T> MakeCopy(const T* data, size_t size)
                {
                    RawData_t<T> rawData;
                    rawData.Copy(data, size);
                    return rawData;
                }
                
                size_t GetSize() const{
                    return mSize;
                }
                
                T* GetData(){
                    return mData.get();
                }
                
                bool empty() const{
                    return GetSize() == 0;
                }
            };
            
            typedef RawData_t<unsigned char> BinaryData_t;
            
            inline UString GenerateTimestamp()
            {
#ifdef BUILD_PLATFORM_WINDOWS
                SYSTEMTIME sm;
                FILETIME fm;
                LARGE_INTEGER currentTime, baseTime;
                
                GetSystemTime(&sm);
                SystemTimeToFileTime(&sm, &fm);
                currentTime.HighPart = fm.dwHighDateTime;
                currentTime.LowPart = fm.dwLowDateTime;
                
                memset(&sm, 0, sizeof(sm));
                sm.wDay = 1;
                sm.wMonth = 1;
                sm.wYear = 1970;
                sm.wSecond = 1;
                
                SystemTimeToFileTime(&sm, &fm);
                
                baseTime.HighPart = fm.dwHighDateTime;
                baseTime.LowPart = fm.dwLowDateTime;
                
                long long secondsLapsed = currentTime.QuadPart - baseTime.QuadPart;
                std::wstringstream strStream;
                strStream << (secondsLapsed)/10000000LL;
                return UString(strStream.str());
#else
                time_t timestamp;
                time(&timestamp);
                
                std::wstringstream sstream;
                sstream << timestamp;
                
                return UString(sstream.str());
#endif
                
            }
            
            inline UString GenerateUUID()
            {
#ifdef BUILD_PLATFORM_WINDOWS                
                RPC_WSTR buffer = NULL;
                UUID uuid;
                UuidCreate(&uuid);
                UuidToStringW(&uuid, &buffer);
                
                UString uuidStr((wchar_t *)buffer);
                RpcStringFreeW(&buffer);
                return uuidStr;
#else
                CFUUIDRef	uuidObj = CFUUIDCreate(nil);
                NSString	*uuidString = (__bridge NSString*)CFUUIDCreateString(nil, uuidObj);
                
                CFRelease(uuidObj);
                
                return UString(uuidString);
#endif                
            }
            
            inline UString GetAppCallbackURL()
            {
                return UString(L"onedrive://");
            }
            
            inline void ParseQueryString(const UString &query, std::map<UString, UString> &paramKeyValues)
            {
                std::vector<UString> wQueryComponents;
                query.Split(wQueryComponents, L"&");
                for(auto param:wQueryComponents)
                {
                    std::vector<UString> wParamKeyValue;
                    param.Split(wParamKeyValue, L"=");
                    if(wParamKeyValue.size() == 2)
                    {
                        paramKeyValues[wParamKeyValue[0]] = wParamKeyValue[1];
                    }
                }
            }
            
            inline void ParseURL(const UString &url, UString &baseURL, UString &queryString, std::map<UString, UString>& paramKeyValues)
            {
                std::vector<UString> queryStrings;
                url.Split(queryStrings, L"?");
                baseURL = queryStrings[0];
                paramKeyValues.clear();
                if(queryStrings.size() > 1)
                {
                    queryString = queryStrings[1];
                    ParseQueryString(queryString, paramKeyValues);
                }
            }
            
            inline void ParseURL(const UString &url, std::map<UString, UString> &paramKeyValues)
            {
                UString baseURL, queryString;
                ParseURL(url, baseURL, queryString, paramKeyValues);
            }
            
            inline UString GetAppDataFolderPath()
            {
                UString strAppFolder;
#ifdef BUILD_PLATFORM_WINDOWS
#else
                const wchar_t *pathDel = L"/";
                NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
                NSString *documentsDirectory = [paths objectAtIndex:0];               
                strAppFolder = UString::fromPlatform(documentsDirectory);
#endif
                //Remove the trailing slash!
                if(strAppFolder[strAppFolder.size() - 1] == pathDel[0])
                {
                    strAppFolder[strAppFolder.size() - 1] = 0;
                }
                
                return strAppFolder;
            }
            
            inline void CreateFoldersOfPath(const UString& path)
            {
#ifdef BUILD_PLATFORM_WINDOWS
#else
                if (![[NSFileManager defaultManager] fileExistsAtPath:path.getPlatformString()])
                {
                    NSError* error;
                    if(  [[NSFileManager defaultManager] createDirectoryAtPath:path.getPlatformString() withIntermediateDirectories:YES attributes:nil error:&error])
                        ;// success
                    else
                    {
                        NewGroup::Core::Utils::Log(UString("Failed to create folder at path: ") + path + UString(". Error (") + UString([error code]) + UString(": ") + UString::fromPlatform([error localizedDescription]));
                    }
                }
#endif
            }
            
            inline void CreateFileWithData(const UString& filePath, const void *data, size_t dataSize)
            {
#ifdef BUILD_PLATFORM_WINDOWS
#else
                NSData* fileData = [NSData dataWithBytesNoCopy:(void *)data length:dataSize freeWhenDone:NO];
                [fileData writeToFile:filePath.getPlatformString() atomically:YES];
#endif
            }
            
            inline void SplitFilePathAndName(const UString& fullPath, UString& filePath, UString& fileName, const wchar_t *del = L"/")
            {
                filePath = fileName = UString();
                auto index = fullPath.rfind(del);
                if(index == UString::npos)
                {
                    fileName = fullPath;
                }
                else
                {
                    filePath = fullPath.substr(0, index + 1);
                    fileName = fullPath.substr(index + 1);
                }
            }
            
            inline void ResizeImage(const void *jpegData, size_t dataSize, size_t newWidth, size_t newHeight, const UString& imagePath)
            {
#ifdef BUILD_PLATFORM_IOS
                UIImage* image = [UIImage imageWithData:[NSData dataWithBytesNoCopy:(void *)jpegData length:dataSize freeWhenDone:NO]];
                if(image.size.width > image.size.height)
                {
                    double aspectRatio = (double)image.size.height/(double)image.size.width;
                    newHeight = ((double)newHeight) * aspectRatio;
                }
                else
                {
                    double aspectRatio = (double)image.size.width/(double)image.size.height;
                    newWidth = ((double)newWidth) * aspectRatio;
                }
                
                auto newSize = CGSizeMake(newWidth, newHeight);
                UIGraphicsBeginImageContext( newSize );
                [image drawInRect:CGRectMake(0,0,newSize.width,newSize.height)];
                UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
                UIGraphicsEndImageContext();
                [UIImageJPEGRepresentation(newImage, 1.0) writeToFile:imagePath.getPlatformString() atomically:YES];
#else
#endif
            }
            
            inline BinaryData_t ReadFileData(const UString& filePath)
            {
#ifdef BUILD_PLATFORM_WINDOWS
#else
                NSData* data = [NSData dataWithContentsOfFile:filePath.getPlatformString()];
                auto rawData = (const unsigned char*)[data bytes];
                
                BinaryData_t fileData;
                fileData.Copy(rawData, [data length]);
                return fileData;
#endif
                
            }
            
            inline UString GetFileExtension(const UString &path)
            {
#ifdef BUILD_PLATFORM_WINDOWS
#else
                return UString::fromPlatform([path.getPlatformString() pathExtension]);
#endif
            }
        }
    }
}

#endif
