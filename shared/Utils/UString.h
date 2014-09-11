#pragma once
#include "PlatformConfig.h"
#include "CommonDefines.h"
#include "Exception.h"
#include <string>
#include <memory>
#include <vector>
#include <sstream>

#if Macintosh
#import <Foundation/NSString.h>
#endif


namespace NewGroup
{
	namespace Core
	{
		typedef std::string UTF8String;
		class UString;
		#ifdef BUILD_PLATFORM_WINDOWS
			typedef std::wstring PlatformString;
			typedef const PlatformString& PlatformStringRef;

		#else
			typedef NSString * PlatformString;
			typedef NSString * PlatformStringRef;
		#endif

		enum 
		{
			kPlatformEncodingUTF16,
			kPlatformEncodingUTF32
		};

		template <int val>
		struct PlatformUTFTraits
		{

		};

		template <>
		struct PlatformUTFTraits<2>
		{
			enum {kPlatformEncoding = kPlatformEncodingUTF16};
			static PlatformString convertToPlatform(const char *utf8String)
			{
				#ifdef BUILD_PLATFORM_WINDOWS
				int length = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8String, -1, NULL, NULL);
				if(length > 0)
				{
					PlatformString strWide;
					strWide.resize(length);
					length = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8String, -1, (wchar_t *)strWide.data(), length);
					return strWide;
				}
				else
				{
					throw Exception("Failed to convert from UTF8 to UTF16");					
				}
				#elif BUILD_PLATFORM_APPLE
                return [NSString stringWithUTF8String:utf8String];
				#endif
			}

			static UTF8String getUTF8String(const UString& str);
			static UString convertFromPlatform(PlatformStringRef platformStr);
			static PlatformString convertToPlatform(const UString& str);
		};

#ifdef BUILD_PLATFORM_APPLE
        template <>
		struct PlatformUTFTraits<4>
		{
			enum {kPlatformEncoding = kPlatformEncodingUTF32};
			static PlatformString convertToPlatform(const char *utf8String)
			{
				return [NSString stringWithUTF8String:utf8String];
			}
            
			static UTF8String getUTF8String(const UString& str);
			static UString convertFromPlatform(PlatformStringRef platformStr);
            static PlatformString convertToPlatform(const UString& str);
		};
                
#endif
		class UString:public std::wstring
		{
		public:
			UString()
			{
			}

            UString(const wchar_t *wString):std::wstring(wString)
            {
                
            }
            
			UString(const std::wstring& wstr):std::wstring(wstr)
			{

			}

			UString(const char *utf8String):std::wstring(fromUtf8(utf8String))
			{
			}

            UString(const std::string &utf8String):std::wstring(fromUtf8(utf8String.c_str()))
			{
			}
#ifndef BUILD_PLATFORM_WINDOWS
			UString(PlatformStringRef platformStr):std::wstring(fromPlatform(platformStr))
			{
			}
            
            inline UString lowerCaseString() const
            {
                return UString::fromPlatform([getPlatformString() lowercaseString]);
            }
#endif
            template <typename T>
            UString(const T& val)
            {
                *this = fromValue(val);
            }
            
			UTF8String getUTF8String() const
			{
				return PlatformUTFTraits<sizeof(wchar_t)>::getUTF8String(*this);
			}

            PlatformString getPlatformString() const
            {
                return PlatformUTFTraits<sizeof(wchar_t)>::convertToPlatform(*this);
            }
            
            static UString fromUtf8(const char *utf8String)
			{
				return PlatformUTFTraits<sizeof(wchar_t)>::convertToPlatform(utf8String);
			}

			static UString fromPlatform(PlatformStringRef platformStr)
			{
				return PlatformUTFTraits<sizeof(wchar_t)>::convertFromPlatform(platformStr);
			}
			
            template <typename T>
            static UString fromValue(const T& val)
            {
                std::wstringstream strStream;
                strStream << val;
                return UString(strStream.str());
            }
            
			void Split(std::vector<UString> &stringVector, const wchar_t *del) const
            {
                size_t initialPos = 0;
                size_t pos1 = 0;
                stringVector.clear();
                while(size() > initialPos && (pos1 = find(del, initialPos)) != std::wstring::npos)
                {
                    if(pos1 > initialPos)
                        stringVector.push_back(substr(initialPos, pos1 - initialPos));
                    initialPos = pos1 + wcslen(del);
                }
                
                if(size() > initialPos)
                {
                    stringVector.push_back(substr(initialPos, std::wstring::npos));
                }
            }
            
            void AppendPathComponent(const UString& component, const wchar_t *del = NULL)
            {
#ifdef BUILD_PLATFORM_WINDOWS
                const wchar_t *pathDel = del ? del : L"\\";
#else
                const wchar_t *pathDel = del ? del : L"/";
#endif
                if((!size() || at(size() - 1) != pathDel[0]) && component[0] != pathDel[0])
                    append(pathDel);
                append(component);
            }
		};

		inline UString PlatformUTFTraits<2>::convertFromPlatform(PlatformStringRef platformStr)
		{
			#ifdef BUILD_PLATFORM_WINDOWS
			return UString(platformStr);
			#else
            NSData* pSData = [platformStr dataUsingEncoding : NSUTF16LittleEndianStringEncoding ];
            return std::wstring ((wchar_t*) [pSData bytes], [pSData length]/sizeof (wchar_t ));
			#endif
		}

		inline UTF8String PlatformUTFTraits<2>:: getUTF8String(const UString& str)
		{
			#ifdef BUILD_PLATFORM_WINDOWS
			int length = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.size(), NULL, NULL, NULL, NULL);
			int size = str.size();
			if(length > 0)
			{
				UTF8String strUTF8;
				strUTF8.resize(length);
				length = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.size(), (char *)strUTF8.data(), length, NULL, NULL);
				return strUTF8;
			}
			else
			{
				throw Exception("Failed to convert from UTF16 to UTF8");					
			}
			#else
            auto platformStr = convertToPlatform(str);
            return UTF8String([platformStr UTF8String]);
			#endif
		}
        
        inline PlatformString PlatformUTFTraits<2>::convertToPlatform(const UString& str)
        {
            #ifdef BUILD_PLATFORM_WINDOWS
            return str;
            #else
            return [[NSString alloc] initWithBytes:str.data()
                                            length:str.size() * sizeof(wchar_t)
                                          encoding:NSUTF16LittleEndianStringEncoding];
            #endif
        }
        
#ifdef BUILD_PLATFORM_APPLE
        inline UTF8String PlatformUTFTraits<4>::getUTF8String(const UString& str)
        {
            auto platformStr = convertToPlatform(str);
            return UTF8String([platformStr UTF8String]);
        }
        
        inline UString PlatformUTFTraits<4>::convertFromPlatform(PlatformStringRef platformStr)
        {
            NSData* pSData = [platformStr dataUsingEncoding : NSUTF32LittleEndianStringEncoding ];
            return std::wstring ((wchar_t*) [pSData bytes], [pSData length]/sizeof (wchar_t ));
        }
        
        inline PlatformString PlatformUTFTraits<4>::convertToPlatform(const UString& str)
        {
            return [[NSString alloc] initWithBytes:str.data()
                                             length:str.size() * sizeof(wchar_t)
                                           encoding:NSUTF32LittleEndianStringEncoding];
        }
        
#endif
        
	}
}