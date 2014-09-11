//
//  DropboxImpl.cpp
//  AccountServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#include "DropboxImpl.h"
#include "CommonUtils.h"
#include "StringConstants.h"
#include "ODLogger.h"
#include <json.h>

using namespace NewGroup::Core;
std::map<UTF8String, UTF8String> DropboxImpl::mDropBoxToIconsMapping;
DropboxImpl::DropboxImpl()
{
    if(mDropBoxToIconsMapping.empty())
    {
        mDropBoxToIconsMapping["folder_app"] = "folder";
        mDropBoxToIconsMapping["folder_gray"] = "folder-grey";
        mDropBoxToIconsMapping["folder_photos"] = "folder-image";
        mDropBoxToIconsMapping["folder_public"] = "folder-remote";
        mDropBoxToIconsMapping["folder_star"] = "folder-favorites";
        mDropBoxToIconsMapping["folder_user"] = "folder-user";
        mDropBoxToIconsMapping["folder"] = "folder";
        mDropBoxToIconsMapping["package"] = "application-x-archive";
        mDropBoxToIconsMapping["page_white_acrobat"] = "application-pdf";
        mDropBoxToIconsMapping["page_white_actionscript"] = "text-x-script";
        mDropBoxToIconsMapping["page_white_c"] = "text-x-csrc";
        mDropBoxToIconsMapping["page_white_code"] = "application-xml";
        mDropBoxToIconsMapping["page_white_compressed"] = "application-x-gzip";
        mDropBoxToIconsMapping["page_white_cplusplus"] = "text-x-c++src";
        mDropBoxToIconsMapping["page_white_csharp"] = "text-x-csharp";
        mDropBoxToIconsMapping["page_white_cup"] = "text-x-java";
        mDropBoxToIconsMapping["page_white_dvd"] = "application-x-cd-image";
        mDropBoxToIconsMapping["page_white_excel"] = "application-ms-excel";
        mDropBoxToIconsMapping["page_white_film"] = "video-x-generic";
        mDropBoxToIconsMapping["page_white_flash"] = "video-x-generic";
        mDropBoxToIconsMapping["page_white_gear"] = "text-x-makefile";
        mDropBoxToIconsMapping["page_white_paint"] = "applications-graphics-2";
        mDropBoxToIconsMapping["page_white_php"] = "application-x-php";
        mDropBoxToIconsMapping["page_white_picture"] = "image-x-generic";
        mDropBoxToIconsMapping["page_white_powerpoint"] = "application-ms-powerpoint";
        mDropBoxToIconsMapping["page_white_ruby"] = "application-x-ruby";
        mDropBoxToIconsMapping["page_white_sound"] = "audio-x-generic";
        mDropBoxToIconsMapping["page_white_text"] = "text-plain";
        mDropBoxToIconsMapping["page_white_vector"] = "image-svg+xml";
        mDropBoxToIconsMapping["page_white_word"] = "application-msword";
    }
}

DropboxImpl::~DropboxImpl()
{
    
}

void DropboxImpl::Authenticate(PlatformView parentView, AuthCallBackProc_t callBack)
{
    //auto v = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, 100, 100)];
    
    auto callBackFunc = std::bind(&DropboxImpl::HandleAuthStage1Response, this, parentView, callBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
    
    WebServicesInterface::ParametersType_t params;
    WebServicesInterface::UserDataType_t userData;
    params[UString(L"oauth_consumer_key")] = UString(Dropbox::kAppBaseKey);
	params[UString(L"oauth_signature_method")] = UString(L"PLAINTEXT");
	params[UString(L"oauth_signature")] = UString(Dropbox::kAppSecret) + UString(L"&");
	params[UString(L"oauth_timestamp")] = Utils::GenerateTimestamp();
	params[UString(L"oauth_nonce")] = Utils::GenerateUUID();
    
    webServiceEngine->SendGetRequest(UString(L"https://api.dropbox.com/1/oauth/request_token"), params, userData);
}

void DropboxImpl::GetDefaultReqParameters(WebServicesInterface::ParametersType_t &params)
{
    params[L"oauth_token"] = mAccessToken;
    params[L"oauth_consumer_key"] = UString(Dropbox::kAppBaseKey);
    params[L"oauth_timestamp"] = Utils::GenerateTimestamp();
    params[L"oauth_nonce"] = Utils::GenerateUUID();
    params[L"oauth_signature_method"] = L"PLAINTEXT";
    params[L"oauth_signature"] = UString(Dropbox::kAppSecret) + UString(L"&") + mTokenSecret;
}

void DropboxImpl::AuthenticateWithToken(const UString& authToken, AuthCallBackProc_t callBack)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, authToken, callBack](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
       {
           if(code == kHTTPResponseOK)
           {
               AccountInfo accInfo;
               ParseAccountInfo(response, accInfo);
               
               /*Json::Reader jsonReader;
               Json::Value rootValue;
               
               jsonReader.parse(response.getUTF8String(), rootValue);
               auto str = rootValue["referral_link"].asString();*/
               
               callBack(kAuthenticationPassed, authToken, accInfo);
               
               Utils::CreateFoldersOfPath(GetThumbnailRootFolder());
               Utils::Log(UString("Folder created: ") + GetThumbnailRootFolder());
           }
           else if(code == kHTTPTokenExpired)
           {
               callBack(kAuthenticationDenied, UString(), AccountInfo());
           }
           else
           {
               callBack(kAuthenticationUnknownErr, UString(), AccountInfo());
           }
       });
    
    Utils::Log(UString("Authenticating with Access Token: ") + authToken);
    
    std::vector<UString> tokenParts;
    authToken.Split(tokenParts, L" ");
    
    if(tokenParts.size() == 3)
    {
        mAccessToken = tokenParts[0];
        mTokenSecret = tokenParts[1];
        mUID = mTokenSecret[2];
    }
    else
    {
        callBack(kAuthenticationUnknownErr, UString(), AccountInfo());
        Utils::Log(UString("Invalid Access Token: ") + authToken);
        return;
    }
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    webServiceEngine->SendGetRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kGetAccountInfoURL), params, WebServicesInterface::UserDataType_t());
}

void DropboxImpl::ParseAccountInfo(const UString& response, AccountInfo& accInfo)
{
    Json::Reader jsonReader;
    Json::Value rootValue;
    jsonReader.parse(response.getUTF8String(), rootValue);
    accInfo.mAuthor = rootValue["display_name"].asString();
    auto email = rootValue["email"].asString();
    
    accInfo.mAccountDescription = accInfo.mAuthor;
    if(!email.empty())
    {
        accInfo.mAccountDescription += UString("[") + UString(email) + UString("]");
    }
    accInfo.mAccountID = rootValue["uid"].asUInt();
    
    auto quotaInfo = rootValue["quota_info"];
    accInfo.mTotalSpace = quotaInfo["quota"].asUInt64();
    
    accInfo.mFreeSpaceLeft = accInfo.mTotalSpace - (quotaInfo["normal"].asUInt64() + quotaInfo["shared"].asUInt64());
}


void DropboxImpl::RequestAccountInfo(AccInfoCallBackProc_t callBack)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
        {
            if(code == kHTTPResponseOK)
            {
                AccountInfo accInfo;
                ParseAccountInfo(response, accInfo);
                
                //Utils::Log(UString(accInfo.mFreeSpaceLeft));
                callBack(accInfo);
                
                /*StartSendingUpdates([this](bool deleted, const UString& checkPoint, const FileProperties &fileProp)
                                    {
                                        Utils::Log(fileProp.mFilePath);
                                    }, UString());*/
                
                /*UploadFile(UString("/Users/rkansal/svnX 1.3.4.dmg"), UString("/testdata/Upload/svnX1.3.4.dmg"), [this](bool){});*/

             }
         });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    webServiceEngine->SendGetRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kGetAccountInfoURL), params, WebServicesInterface::UserDataType_t());
}

void DropboxImpl::ParseMetaData(Json::Value &fileMetadata, FileProperties& fileProp, const UString& fileID)
{
    fileProp.mFileID = fileID;
    assert(!fileProp.mFileID.empty());
    fileProp.mFileSize = fileMetadata["bytes"].asUInt64();
    fileProp.mFilePath = fileMetadata["path"].asString();
    fileProp.mIsFolder = fileMetadata["is_dir"].asBool();
    fileProp.mFileRev = fileMetadata["rev"].asString();
    fileProp.mMimeType = fileMetadata["mime_type"].asString();
    fileProp.mLastModifiedTime = DateTime(UString(fileMetadata["modified"].asString()), UString(Dropbox::kDateTimeFormat));
    
    std::vector<UString> pathComponents;
    fileProp.mFilePath.Split(pathComponents, L"/");
    
    auto rit = fileProp.mFileID.rbegin();
    fileProp.mParentID = UString(L"/");
    while (rit != fileProp.mFileID.rend())
    {
        if(*rit == '/')
        {
            ++rit;
            if(rit != fileProp.mFileID.rend())
            {
                fileProp.mParentID.assign(fileProp.mFileID, 0, fileProp.mFileID.rend() - rit);
            }
            break;
        }
        ++rit;
    }

    fileProp.mIsRootElement = (fileProp.mParentID == UString(L"/"));
    
    if(!pathComponents.empty())
        fileProp.mFileName = pathComponents[pathComponents.size() - 1];
    
    bool thumb_exists = fileMetadata["thumb_exists"].asBool();
    if(thumb_exists)
    {
        /*DownloadFile(entry[0].asString(), fileProp.mFilePath, [this](const UString &remotePath, const UString &localPath, const UString& fileRev)
         {
         Utils::Log("File download complete");
         Utils::Log(remotePath);
         Utils::Log(localPath);
         Utils::Log(fileRev);
         });*/
        /*DownloadThumbnail(fileProp.mFileID, fileProp.mFilePath, 256, 256, [this](const UString &remotePath, const UString &localPath)
         {
         Utils::Log(remotePath + UString(" thumbnail completed ") + localPath);
         });*/
    }
    else if(fileMetadata.isMember("icon"))
        fileProp.mThumbnail = GetIconName(fileMetadata["icon"].asString());
}

void DropboxImpl::StartSendingUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
        {
            if(code == kHTTPResponseOK)
            {
                Json::Reader jsonReader;
                Json::Value rootValue;
                jsonReader.parse(response.getUTF8String(), rootValue);
                
                auto hasMore = rootValue["has_more"].asBool();
                auto cursor = rootValue["cursor"].asString();
                auto entries = rootValue["entries"];
                
                auto index = 1;
                if(entries.size() == 0)
                {
                    FileProperties fileProp;
                    callBack(false, cursor, fileProp);
                }
                for(auto entry:entries)
                {
                    bool isDeleted = false;
                    FileProperties fileProp;
                    auto fileMetadata = entry[1];
                    UString checkPoint;
                    
                    if(fileMetadata.isNull())
                    {
                        isDeleted = true;
                    }
                    else
                    {
                        ParseMetaData(fileMetadata, fileProp, entry[0].asString());
                    }
                    
                    if(entries.size() == index)
                        checkPoint = cursor;
                    
                    callBack(isDeleted, checkPoint, fileProp);
                    ++index;
                }
                
                if(hasMore)
                {
                    StartSendingUpdates(callBack, cursor);
                }
            }
        });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    if(!checkpoint.empty())
    {
        params[L"cursor"] = checkpoint;
    }
    
    webServiceEngine->SendPostRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kDeltaReqURL), BinaryData_t(), params, WebServicesInterface::UserDataType_t());
}

void DropboxImpl::AuthenticateStage2(PlatformView parentView, AuthCallBackProc_t callBack, const UString &oauthToken, const UString &oauthSecret)
{
    //PlatformView parentView, const UString& url, const UString &callBackUrl, AuthCallBackProc_t callback
    
    UString baseURL(L"https://www.dropbox.com/1/oauth/authorize");
    
	auto callbackURI = Utils::GetAppCallbackURL();
    
	WebServicesInterface::ParametersType_t params;
    params[UString(L"oauth_token")] = oauthToken;
    params[UString(L"oauth_callback")] = callbackURI;
    //params[UString(L"display")] = UString(L"mobile");
    
    auto callBackFunc = std::bind(&DropboxImpl::HandleAuthStage1Response, this, (PlatformView)NULL, callBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
    /**/
	auto strURL = webServiceEngine->EncodeURL(baseURL, params);
    UString token(L"oauth_token");
    mOauthDlgPtr.reset(new OAuthDialog(parentView, strURL, callbackURI, token, [this, callBack, oauthSecret](bool success, const UString& requestToken)->void{
        if(success && requestToken.size())
        {
            UString reqURL = CombineURL(Dropbox::kBaseAPIURL, Dropbox::kGetTokenURL);
            WebServicesInterface::ParametersType_t params;
            params[UString(L"oauth_consumer_key")] = UString(Dropbox::kAppBaseKey);
            params[UString(L"oauth_token")] = requestToken;
            params[UString(L"oauth_signature_method")] = UString(L"PLAINTEXT");;
            params[UString(L"oauth_signature")] = UString(Dropbox::kAppSecret) + L"&" + oauthSecret;
            params[UString(L"oauth_timestamp")] = Utils::GenerateTimestamp();
            params[UString(L"oauth_nonce")] = Utils::GenerateUUID();
            
            auto callBackFunc = std::bind(&DropboxImpl::HandleAuthStage2Response, this, callBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
            webServiceEngine->SendPostRequest(reqURL, BinaryData_t(), params, WebServicesInterface::UserDataType_t());
        }
        else
        {
            callBack(kAuthenticationDenied, UString(), AccountInfo());
        }
        
    }));
    
}

void DropboxImpl::HandleAuthStage1Response(PlatformView parentView, AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
{
    Utils::Log(UString("Code:") + UString(code) + UString(", response: ") + response);
    if(code == kHTTPResponseOK)
    {
        std::vector<UString> keyValueComponents;
        response.Split(keyValueComponents, L"&");
        
        UString oauthToken, oauthSecret;
        for(auto keyValue:keyValueComponents)
        {
            std::vector<UString> tokenComponents;
            keyValue.Split(tokenComponents, L"=");
            if(UString(L"oauth_token") == tokenComponents[0])
                oauthToken = tokenComponents[1];
            else if(UString(L"oauth_token_secret") == tokenComponents[0])
                oauthSecret = tokenComponents[1];
            for(auto component:tokenComponents)
            {
                Utils::Log(component);
            }
        }
        
        AuthenticateStage2(parentView, callBack, oauthToken, oauthSecret);
    }
    else
    {
        callBack(kAuthenticationUnknownErr, UString(), AccountInfo());
    }
    Utils::Log("After stage 2");
}

void DropboxImpl::HandleAuthStage2Response(AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
{
    std::map<UString, UString> paramKeyValues;
    if(code == kHTTPResponseOK)
    {
        Utils::Log(response);
        Utils::ParseQueryString(response, paramKeyValues);
        mAccessToken = paramKeyValues[UString(L"oauth_token")];
        mTokenSecret = paramKeyValues[UString(L"oauth_token_secret")];
        mUID = paramKeyValues[UString(L"uid")];
        
        //Utils::Log(mAccessToken + UString(" ") + mTokenSecret + UString(" ") + mUID);
        AuthenticateWithToken(mAccessToken + UString(" ") + mTokenSecret + UString(" ") + mUID, callBack);
        //callBack(kAuthenticationPassed, mAccessToken + UString(L"\n") + mTokenSecret);
    }
    else
    {
        callBack(kAuthenticationUnknownErr, UString(), AccountInfo());
    }
}

UString DropboxImpl::GetServiceRootFolder() const
{
    auto rootFolder = Utils::GetAppDataFolderPath();
    rootFolder.AppendPathComponent("Dropbox");
    rootFolder.AppendPathComponent(mUID);    
    return rootFolder;
}

UString DropboxImpl::GetThumbnailRootFolder() const
{
    auto rootFolder = GetServiceRootFolder();
    rootFolder.AppendPathComponent("ThumbnailCache");
    return rootFolder;
}

UString DropboxImpl::GetDownloadRootFolder() const
{
    auto rootFolder = GetServiceRootFolder();
    rootFolder.AppendPathComponent("DownloadCache");
    return rootFolder;
}

void DropboxImpl::DownloadThumbnail(const UString &fileID, const UString &path, int width, int height, ThumbnailCallBackProc_t callback)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(WebServicesInterface::CallBackProc_t(), [this, callback, path, width, height, fileID](int code, const WebServicesInterface::UserDataType_t& userData, const unsigned char* response, size_t size, const WebServicesInterface::HeaderListType_t& headers)
        {
            if(code == kHTTPResponseOK)
            {
                UString fullPath, name;
                Utils::SplitFilePathAndName(path, fullPath, name);
                auto filePath = GetThumbnailRootFolder() + fullPath;
                Utils::CreateFoldersOfPath(filePath);
                filePath = filePath + Utils::GenerateUUID() + UString(".jpg");
                Utils::ResizeImage(response, size, width, height, filePath);
                callback(fileID, filePath);
                Utils::Log(UString("Thumbnail of file ") + path + UString(" saved to ") + filePath);
            }
        });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    auto maxDim = std::max(width, height);
    
    const char *strSize = "xl";
    if(maxDim <= 32)
        strSize = "xs";
        else if(maxDim <= 64)
            strSize = "s";
            else if(maxDim <= 128)
                strSize = "m";
                else if(width <= 640 && height <= 480)
                    strSize = "l";
    
    params["size"] = strSize;
    
    webServiceEngine->SendGetRequest(UString(Dropbox::kBaseContentAPIURL) + UString(Dropbox::kThumbnailReqURL) + UString(Dropbox::kDropboxRoot) + fileID, params, WebServicesInterface::UserDataType_t());
}


void DropboxImpl::DownloadFile(const UString &fileID, const UString &path, FileCallBackProc_t callback)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(WebServicesInterface::CallBackProc_t(), [this, callback, path, fileID](int code, const WebServicesInterface::UserDataType_t& userData, const unsigned char* response, size_t size, const WebServicesInterface::HeaderListType_t& headers)
        {
            if(code == kHTTPResponseOK)
            {
                UString rev, actualFileName;
                UString actualFilePath;
                for(auto header:headers)
                {
                    if(header.find(L"x-dropbox-metadata:") == 0)
                    {
                        Json::Reader jsonReader;
                        Json::Value rootValue;
                        UString jsonStr = header;
                        jsonStr.erase(0, sizeof("x-dropbox-metadata:"));
                        Utils::Log(jsonStr);
                        jsonReader.parse(jsonStr.getUTF8String(), rootValue);
                        rev = rootValue["rev"].asString();
                    }
                }
                
                Utils::SplitFilePathAndName(path, actualFilePath, actualFileName);
                auto filePath = GetDownloadRootFolder() + actualFilePath;
                Utils::CreateFoldersOfPath(filePath);
                filePath += Utils::GenerateUUID() + L"_" + actualFileName;
                Utils::CreateFileWithData(filePath, response, size);
                callback(fileID, filePath, rev);
            }
            
        }, WebServicesInterface::ProgressProc_t(), true);
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    webServiceEngine->SendGetRequest(UString(Dropbox::kBaseContentAPIURL) + UString(Dropbox::kFileGetReqURL) + UString(Dropbox::kDropboxRoot) + fileID, params, WebServicesInterface::UserDataType_t());
}

void DropboxImpl::UploadFile(const UString &localFilePath, const UString &remoteParent, FileUploadCallBackProc_t callback)
{
    std::vector<UString> pathComponents;
    localFilePath.Split(pathComponents, L"/");
    
    if(!pathComponents.size())
    {
        Utils::Log(UString("DropboxImpl::UploadFile: Invalid local path in upload: ") + localFilePath);
        return;
    }
    
    UString remoteFilePath = remoteParent + L"/" + pathComponents[pathComponents.size() - 1];
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, localFilePath, remoteFilePath](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
        {
            if(code == kHTTPResponseOK)
            {
                Utils::Log(UString("Upload complete for ") + localFilePath);
                FileProperties fileProp;
                Json::Reader jsonReader;
                Json::Value fileMetadata;
                jsonReader.parse(response.getUTF8String(), fileMetadata);
                ParseMetaData(fileMetadata, fileProp, UString(fileMetadata["path"].asString()).lowerCaseString());
                callback(true, fileProp);
            }
            else
            {
                callback(false, FileProperties());
            }
        });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    auto fileData = Utils::ReadFileData(localFilePath);
    webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
    webServiceEngine->SendPutRequest(UString(Dropbox::kBaseContentAPIURL) + UString(Dropbox::kFilePutReqURL) + UString(Dropbox::kDropboxRoot) + remoteFilePath, fileData, params, WebServicesInterface::UserDataType_t());
}

void DropboxImpl::GeneratePublicURL(const UString &fileID, FileURLCallBackProc_t callback)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, fileID](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               Json::Reader jsonReader;
                                                                               Json::Value rootValue;
                                                                               jsonReader.parse(response.getUTF8String(), rootValue);
                                                                               UString sharedURL = rootValue["url"].asString();
                                                                               
                                                                               Utils::Log(UString("Shared link for ") + fileID + UString(" is: ") + sharedURL);
                                                                               callback(fileID, sharedURL);
                                                                           }
                                                                           else
                                                                           {
                                                                               Utils::Log(UString("Shared link for ") + fileID + UString(" failed!. Error: ") + UString(code));
                                                                               callback(fileID, UString());
                                                                           }
                                                                       });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
    webServiceEngine->SendPostRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kGetPublicURL) + UString(Dropbox::kDropboxRoot) + fileID, BinaryData_t(), params, WebServicesInterface::UserDataType_t());
}

UString DropboxImpl::GetIconName(const UTF8String& dropboxIcon)
{
    UString appIconId;
        
    auto iconName = mDropBoxToIconsMapping.find(dropboxIcon);
    
    
    if(iconName != mDropBoxToIconsMapping.end())
    {
        appIconId = iconName->second;
    }
    else
    {
        appIconId = "application-x-zerosize";
        Utils::Log(UString("Could not convert dropbox icon: ") + UString(dropboxIcon));
    }
    
    return appIconId;
}

void DropboxImpl::CopyItem(const UString &srcfileID, const UString &srcfilePath, const UString &remoteParent, CopyOperationCallbackProc_t callback){
    std::vector<UString> pathComponents;
    srcfilePath.Split(pathComponents, L"/");
    
    if(!pathComponents.size())
    {
        Utils::Log(UString("DropboxImpl::CopyItem: Invalid source file path: ") + srcfileID);
        return;
    }
    
    UString destFileLocation = remoteParent + L"/" + pathComponents[pathComponents.size() - 1];
    
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, srcfileID, destFileLocation](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               FileProperties fileProp;
                                                                               Json::Reader jsonReader;
                                                                               Json::Value fileMetadata;
                                                                               jsonReader.parse(response.getUTF8String(), fileMetadata);
                                                                               ParseMetaData(fileMetadata, fileProp, destFileLocation);
                                                                               callback(true, fileProp);
                                                                           }
                                                                           else
                                                                           {
                                                                               Utils::Log(UString("Copy operation for ") + srcfileID + UString(" failed!. Error: ") + UString(code));
                                                                               callback(false, FileProperties());
                                                                           }
                                                                       });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    params["root"] = Dropbox::kDropboxString;
    params["from_path"] = srcfileID;
    params["to_path"] = destFileLocation;
    
    webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
    webServiceEngine->SendPostRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kCopyFileURL), BinaryData_t(), params, WebServicesInterface::UserDataType_t());

}

void DropboxImpl::MoveItem(const UString &srcfileID, const UString &srcfilePath, const UString &remoteParent, MoveOperationCallbackProc_t callback){
    std::vector<UString> pathComponents;
    srcfilePath.Split(pathComponents, L"/");
    
    if(!pathComponents.size())
    {
        Utils::Log(UString("DropboxImpl::MoveItem: Invalid source file path: ") + srcfileID);
        return;
    }
    
    UString destFileLocation = remoteParent + L"/" + pathComponents[pathComponents.size() - 1];
    
    MoveItemToLocation(srcfileID, destFileLocation, callback);
}

void DropboxImpl::MoveItemToLocation(const UString &srcfileID, const UString &destFileLocation, MoveOperationCallbackProc_t callback){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, srcfileID, destFileLocation](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               
                                                                               callback(true, destFileLocation);
                                                                           }
                                                                           else
                                                                           {
                                                                               Utils::Log(UString("Move operation for ") + srcfileID + UString(" failed!. Error: ") + UString(code));
                                                                               callback(false, UString());
                                                                           }
                                                                       });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    params["root"] = Dropbox::kDropboxString;
    params["from_path"] = srcfileID;
    params["to_path"] = destFileLocation;
    
    webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
    webServiceEngine->SendPostRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kMoveFileURL), BinaryData_t(), params, WebServicesInterface::UserDataType_t());
}

void DropboxImpl::RenameItem(const UString &srcfileID, const UString &destFileLocation, const UString& newName, MoveOperationCallbackProc_t callback){
    MoveItemToLocation(srcfileID, destFileLocation + UString(L"/") + newName, callback);
}

void DropboxImpl::DeleteItem(const UString &fileID, FileOperationCallbackProc_t callback){
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, fileID](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               
                                                                               callback(true);
                                                                           }
                                                                           else
                                                                           {
                                                                               Utils::Log(UString("Delete operation for ") + fileID + UString(" failed!. Error: ") + UString(code));
                                                                               callback(false);
                                                                           }
                                                                       });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    params["root"] = Dropbox::kDropboxString;
    params["path"] = fileID;
    
    webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
    webServiceEngine->SendPostRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kDeleteFileURL), BinaryData_t(), params, WebServicesInterface::UserDataType_t());
}

void DropboxImpl::CreateFolder(const UString& destFileId, const UString &folderName, FileOperationCallbackProc_t callback){
    auto folderPathPath = destFileId + UString("/") + folderName;
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, folderPathPath](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               
                                                                               callback(true);
                                                                           }
                                                                           else
                                                                           {
                                                                               Utils::Log(UString("Create folder operation for ") + folderPathPath + UString(" failed!. Error: ") + UString(code));
                                                                               callback(false);
                                                                           }
                                                                       });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    params["root"] = Dropbox::kDropboxString;
    params["path"] = folderPathPath;
    
    webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
    webServiceEngine->SendPostRequest(UString(Dropbox::kBaseAPIURL) + UString(Dropbox::kCreateFolderURL), BinaryData_t(), params, WebServicesInterface::UserDataType_t());
}
