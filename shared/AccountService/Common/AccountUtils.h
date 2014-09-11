//
//  AccountUtils.h
//  AccountServices
//
//  Created by Ruchin Kansal on 25/05/13.
//  Copyright (c) 2013 Ruchin Kansal. All rights reserved.
//

#ifndef AccountServices_AccountUtils_h
#define AccountServices_AccountUtils_h

#include "CommonDefines.h"
#include "UString.h"
#include <set>
#include "CommonUtils.h"

#if BUILD_PLATFORM_APPLE
#if TARGET_OS_IPHONE
#import <MobileCoreServices/MobileCoreServices.h>
#else
#import <CoreServices/CoreServices.h>
#endif
#endif

namespace NewGroup
{
    namespace Core
    {
        namespace Utils
        {
            inline UString GetMimeTypeFromFileExtension(const UString &ext)
            {
#ifdef BUILD_PLATFORM_WINDOWS
#else
                CFStringRef fileExtension = (__bridge CFStringRef)(ext.getPlatformString());
                CFStringRef UTI = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, fileExtension, NULL);
                CFStringRef MIMEType = UTTypeCopyPreferredTagWithClass(UTI, kUTTagClassMIMEType);
                CFRelease(UTI);
                NSString *MIMETypeString = (__bridge_transfer NSString *)MIMEType;
                
                if([MIMETypeString length] == 0)
                {
                    MIMETypeString = @"application/unknown";
                }
                
                return UString::fromPlatform(MIMETypeString);
#endif
            }
            
            inline UString GetIconNameFromExtension(const UString &extension)
            {
                static std::map<UString, UString> availableIcons;
                if(availableIcons.empty())
                {
                    availableIcons[L"ai"] = L"application-illustrator";
                    availableIcons[L"svg"] = L"application-illustrator";
                    
                    availableIcons[L"ppt"] = L"application-ms-powerpoint";
                    availableIcons[L"pptx"] = L"application-ms-powerpoint";
                    availableIcons[L"pps"] = L"application-ms-powerpoint";
                    availableIcons[L"pptm"] = L"application-ms-powerpoint";
                    availableIcons[L"potx"] = L"application-ms-powerpoint";
                    availableIcons[L"potm"] = L"application-ms-powerpoint";
                    availableIcons[L"ppam"] = L"application-ms-powerpoint";
                    availableIcons[L"ppsx"] = L"application-ms-powerpoint";
                    availableIcons[L"ppsm"] = L"application-ms-powerpoint";
                    availableIcons[L"sldx"] = L"application-ms-powerpoint";
                    availableIcons[L"sldm"] = L"application-ms-powerpoint";
                    
                    availableIcons[L"doc"] = L"application-msword";
                    availableIcons[L"docx"] = L"application-msword";
                    availableIcons[L"docm"] = L"application-msword";
                    availableIcons[L"dotx"] = L"application-msword";
                    availableIcons[L"dotm"] = L"application-msword";
                    availableIcons[L"dot"] = L"application-msword";
                    
                    availableIcons[L"js"] = L"application-javascript";
                    availableIcons[L"pdf"] = L"application-pdf";
                    availableIcons[L"php"] = L"application-x-php";
                    
                    availableIcons[L"xls"] = L"application-ms-excel";
                    availableIcons[L"xlsx"] = L"application-ms-excel";
                    availableIcons[L"xlsm"] = L"application-ms-excel";
                    availableIcons[L"xltx"] = L"application-ms-excel";
                    availableIcons[L"xltm"] = L"application-ms-excel";
                    availableIcons[L"xlsb"] = L"application-ms-excel";
                    availableIcons[L"xlam"] = L"application-ms-excel";
                    availableIcons[L"xll"] = L"application-ms-excel";
                    availableIcons[L"csv"] = L"application-ms-excel";
                    
                    availableIcons[L"tar"] = L"application-x-archive";
                    
                    availableIcons[L"rar"] = L"application-x-gzip";
                    availableIcons[L"zip"] = L"application-x-gzip";
                    availableIcons[L"7z"] = L"application-x-gzip";
                    
                    availableIcons[L"psd"] = L"applications-graphics-2";
                    availableIcons[L"h"] = L"text-x-c++hdr";
                    availableIcons[L"hpp"] = L"text-x-c++hdr";
                    
                    availableIcons["cpp"] = L"text-x-c++src";
                    availableIcons["cc"] = L"text-x-c++src";
                    availableIcons["cs"] = L"text-x-csharp";
                    availableIcons["c"] = L"text-x-csrc";
                    
                    availableIcons["java"] = L"text-x-java";
                    
                    availableIcons["mk"] = L"text-x-makefile";
                    
                    availableIcons["m"] = L"text-x-objchdr";
                    availableIcons["mm"] = L"text-x-objchdr";
                    
                    availableIcons["py"] = L"text-x-python";
                    availableIcons["rb"] = L"application-x-ruby";
                    
                    availableIcons["xml"] = L"application-xml";
                    availableIcons["html"] = L"application-xml";
                    availableIcons["htm"] = L"application-xml";
                    
                    availableIcons[L"mdb"] = L"application-ms-access";
                    availableIcons[L"accdb"] = L"application-ms-access";
                    availableIcons[L"accde"] = L"application-ms-access";
                    availableIcons[L"accdt"] = L"application-ms-access";
                    availableIcons[L"accdr"] = L"application-ms-access";
                    
                }
                
                auto found = availableIcons.find(extension.lowerCaseString());
                if(found == availableIcons.end())
                    return UString();
                else
                    return found->second;
            }
            
            inline std::string GetIconNameFromMimeType(const std::string &mimeType)
            {
                static std::set<std::string> availableIcons;
                if(availableIcons.empty())
                {
                    availableIcons.insert("application-illustrator");
                    availableIcons.insert("application-javascript");
                    availableIcons.insert("application-ms-access");
                    availableIcons.insert("application-ms-excel");
                    availableIcons.insert("application-ms-powerpoint");
                    availableIcons.insert("application-msword");
                    availableIcons.insert("application-pdf");
                    availableIcons.insert("application-vnd.ms-access");
                    availableIcons.insert("application-vnd.ms-excel");
                    availableIcons.insert("application-vnd.ms-powerpoint");
                    availableIcons.insert("application-x-archive");
                    availableIcons.insert("application-x-cd-image");
                    availableIcons.insert("application-x-gzip");
                    availableIcons.insert("application-x-php");
                    availableIcons.insert("application-x-ruby");
                    availableIcons.insert("application-x-zerosize");
                    availableIcons.insert("application-xml");
                    availableIcons.insert("applications-graphics-2");
                    availableIcons.insert("audio-x-generic");
                    availableIcons.insert("folder");
                    availableIcons.insert("image-svg+xml");
                    availableIcons.insert("image-x-generic");
                    availableIcons.insert("text-plain");
                    availableIcons.insert("text-x-c++hdr");
                    availableIcons.insert("text-x-c++src");
                    availableIcons.insert("text-x-csharp");
                    availableIcons.insert("text-x-csrc");
                    availableIcons.insert("text-x-java");
                    availableIcons.insert("text-x-makefile");
                    availableIcons.insert("text-x-objchdr");
                    availableIcons.insert("text-x-python");
                    availableIcons.insert("text-x-script");
                    availableIcons.insert("unknown");
                    availableIcons.insert("video-x-generic");
                }
                
                std::string appIconId;
                if(mimeType.find("video") !=std::string::npos)
                {
                    appIconId = "video-x-generic";
                }
                else if(mimeType.find("audio") !=std::string::npos)
                {
                    appIconId = "audio-x-generic";
                }
                else if(mimeType.find("text/") !=std::string::npos)
                {
                    appIconId = "text-plain";
                }
                else
                {
                    size_t pos = mimeType.find("/",0);
                    
                    if(pos != std::string::npos)
                    {
                        UTF8String tmp = mimeType;
                        tmp.replace(pos, 1, "-");
                        appIconId = tmp;
                    }
                    
                    if(availableIcons.find(appIconId) == availableIcons.end())
                    {
                        appIconId = "application-x-zerosize";
                    }
                }
                
                return appIconId;
            }

        }
    }
}


#endif
