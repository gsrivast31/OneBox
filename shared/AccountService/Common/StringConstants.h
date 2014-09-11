//
//  StringConstants.h
//  AccountServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef AccountServices_StringConstants_h
#define AccountServices_StringConstants_h
#include <string>

namespace NewGroup
{
    namespace Core
    {
        inline std::wstring CombineURL(const wchar_t* base, const wchar_t* path)
        {
            return std::wstring(base) + std::wstring(path);
        }
        
        namespace Dropbox
        {
            const wchar_t* const kBaseAPIURL = L"https://api.dropbox.com/1";
            const wchar_t* const kBaseContentAPIURL = L"https://api-content.dropbox.com/1";
            const wchar_t* const kAppBaseKey = L"smhiy7mkt4t6i4q";
            const wchar_t* const kAppSecret = L"hll3zq0f580v39r";
            
            const wchar_t* const kReqTokenURL = L"/oauth/request_token";
            const wchar_t* const kDoAuthURL = L"/oauth/authorize";
            const wchar_t* const kGetTokenURL = L"/oauth/access_token";
            const wchar_t* const kGetAccountInfoURL = L"/account/info";
            const wchar_t* const kDeltaReqURL = L"/delta";
            const wchar_t* const kThumbnailReqURL = L"/thumbnails";
            const wchar_t* const kDropboxRoot = L"/dropbox";
            const wchar_t* const kFileGetReqURL = L"/files";
            const wchar_t* const kFilePutReqURL = L"/files_put";
            const wchar_t* const kGetPublicURL = L"/shares";
            const wchar_t* const kCopyFileURL = L"/fileops/copy";
            const wchar_t* const kMoveFileURL = L"/fileops/move";
            const wchar_t* const kDeleteFileURL = L"/fileops/delete";
            const wchar_t* const kCreateFolderURL = L"/fileops/create_folder";
            const wchar_t* const kDropboxString = L"dropbox";
            const wchar_t* const kDateTimeFormat = L"%a, %d %b %Y %H:%M:%S %z";
        }
        namespace GDrive
        {
            const wchar_t* const kClient_id = L"675405429907.apps.googleusercontent.com";
            const wchar_t* const kClientSecret = L"dpRkvbcPVfiQ8rOu3ez-VsyR";
            const wchar_t* const kDateTimeFormat = L"%Y-%m-%dT%H:%M:%S.%f";
            const wchar_t* const kFileOpsURL = L"https://www.googleapis.com/drive/v2/files/";
            const wchar_t* const kFileUploadURL = L"https://www.googleapis.com/upload/drive/v2/files/";
            const wchar_t* const kFileCopyURL = L"/copy";
            const wchar_t* const kChildrenURL = L"/children";
            const wchar_t* const kPermissionsURL = L"/permissions";
        }
        
        namespace SkyDrive
        {
            const wchar_t* const kClientID = L"00000000400F7108";
            const wchar_t* const kAppSecret = L"T5xHcAzWFVWeax9bHxxN2rGmdP0f8NYi";
            const wchar_t* const kBaseAPIURL = L"https://apis.live.net/v5.0/";
            const wchar_t* const kGetAccountInfoURL = L"me";
            const wchar_t* const kListFolderContentURL = L"/files";
            const wchar_t* const kThumbnailReqURL = L"skydrive/get_item_preview";
            const wchar_t* const kGetPublicURL = L"/shared_read_link";
            const wchar_t* const kFileGetReqURL = L"/content";
            
        }
    }
}

#endif
