//
//  SkyDriveImpl.cpp
//  AccountServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#include "SkyDriveImpl.h"
#include "CommonUtils.h"
#include "AccountUtils.h"
#include "StringConstants.h"
#include "ODLogger.h"
#include <json.h>
#include "OAuthDialog.h"

using namespace NewGroup::Core;
std::map<UTF8String, UTF8String> SkyDriveImpl::mSkyDriveToIconsMapping;
SkyDriveImpl::SkyDriveImpl()
{
    if(mSkyDriveToIconsMapping.empty())
    {
        mSkyDriveToIconsMapping["folder_app"] = "folder";
        mSkyDriveToIconsMapping["folder_gray"] = "folder-grey";
        mSkyDriveToIconsMapping["folder_photos"] = "folder-image";
        mSkyDriveToIconsMapping["folder_public"] = "folder-remote";
        mSkyDriveToIconsMapping["folder_star"] = "folder-favorites";
        mSkyDriveToIconsMapping["folder_user"] = "folder-user";
        mSkyDriveToIconsMapping["folder"] = "folder";
        mSkyDriveToIconsMapping["package"] = "application-x-archive";
        mSkyDriveToIconsMapping["page_white_acrobat"] = "application-pdf";
        mSkyDriveToIconsMapping["page_white_actionscript"] = "text-x-script";
        mSkyDriveToIconsMapping["page_white_c"] = "text-x-csrc";
        mSkyDriveToIconsMapping["page_white_code"] = "application-xml";
        mSkyDriveToIconsMapping["page_white_compressed"] = "application-x-gzip";
        mSkyDriveToIconsMapping["page_white_cplusplus"] = "text-x-c++src";
        mSkyDriveToIconsMapping["page_white_csharp"] = "text-x-csharp";
        mSkyDriveToIconsMapping["page_white_cup"] = "text-x-java";
        mSkyDriveToIconsMapping["page_white_dvd"] = "application-x-cd-image";
        mSkyDriveToIconsMapping["page_white_excel"] = "application-ms-excel";
        mSkyDriveToIconsMapping["page_white_film"] = "video-x-generic";
        mSkyDriveToIconsMapping["page_white_flash"] = "video-x-generic";
        mSkyDriveToIconsMapping["page_white_gear"] = "text-x-makefile";
        mSkyDriveToIconsMapping["page_white_paint"] = "applications-graphics-2";
        mSkyDriveToIconsMapping["page_white_php"] = "application-x-php";
        mSkyDriveToIconsMapping["page_white_picture"] = "image-x-generic";
        mSkyDriveToIconsMapping["page_white_powerpoint"] = "application-ms-powerpoint";
        mSkyDriveToIconsMapping["page_white_ruby"] = "application-x-ruby";
        mSkyDriveToIconsMapping["page_white_sound"] = "audio-x-generic";
        mSkyDriveToIconsMapping["page_white_text"] = "text-plain";
        mSkyDriveToIconsMapping["page_white_vector"] = "image-svg+xml";
        mSkyDriveToIconsMapping["page_white_word"] = "application-msword";
    }
}

SkyDriveImpl::~SkyDriveImpl()
{
    
}

void SkyDriveImpl::Authenticate(PlatformView parentView, AuthCallBackProc_t callBack)
{
    auto callBackFunc = std::bind(&SkyDriveImpl::HandleAuthStage1Response, this, parentView, callBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
    
    WebServicesInterface::ParametersType_t params;
    WebServicesInterface::UserDataType_t userData;
    
    auto callbackURI = UString(L"https://login.live.com/oauth20_desktop.srf");
    
    params[UString(L"client_id")] = UString(SkyDrive::kClientID);
	params[UString(L"response_type")] = UString(L"code");
	params[UString(L"redirect_uri")] = callbackURI;
	params[UString(L"scope")] = UString("wl.basic wl.offline_access wl.signin wl.emails wl.skydrive_update");
    auto strURL = webServiceEngine->EncodeURL(UString(L"https://login.live.com/oauth20_authorize.srf"), params);
	;
    
    OAuthDialog::ClearCookieForUrl(L"http://login.live.com");
    OAuthDialog::ClearCookieForUrl(L"https://login.live.com");
    mOauthDlgPtr.reset(new OAuthDialog(parentView, strURL, callbackURI, L"code", [this, webServiceEngine, callBack, callbackURI](bool success, const UString& requestToken)->void{
       if(success && requestToken.size())
        {
           
            UString reqURL = L"https://login.live.com/oauth20_token.srf";
            WebServicesInterface::ParametersType_t params, noparam;
            params[UString(L"code")] = requestToken;
            params[UString(L"redirect_uri")] = callbackURI;
            params[UString(L"client_id")] = UString(SkyDrive::kClientID);;
            params[UString(L"grant_type")] = UString(L"authorization_code");
            params[UString(L"client_secret")] = UString(SkyDrive::kAppSecret);
            UString binarydata = webServiceEngine->EncodeURL(UString(L""), params);
            webServiceEngine->SetRequestHeader("Content-Type", UString("application/x-www-form-urlencoded"));
            webServiceEngine->SendPostRequest(reqURL, binarydata, noparam, WebServicesInterface::UserDataType_t());
//
       }
        else
        {
            callBack(kAuthenticationDenied, UString(), AccountInfo());
        }
        
    }));
}

void SkyDriveImpl::GetDefaultReqParameters(WebServicesInterface::ParametersType_t &params)
{
    params[L"access_token"] = mAccessToken;
}

void SkyDriveImpl::AuthenticateWithToken(const UString& authToken, AuthCallBackProc_t callBack)
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
    
    if(tokenParts.size() == 2)
    {
        mAccessToken = tokenParts[0];
        mRefreshToken = tokenParts[1];
    }
    else
    {
        callBack(kAuthenticationUnknownErr, UString(), AccountInfo());
        Utils::Log(UString("Invalid Access Token: ") + authToken);
        return;
    }
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);

    webServiceEngine->SendGetRequest(UString(SkyDrive::kBaseAPIURL) + UString(SkyDrive::kGetAccountInfoURL), params, WebServicesInterface::UserDataType_t());
}

void SkyDriveImpl::ParseAccountInfo(const UString& response, AccountInfo& accInfo)
{
    Json::Reader jsonReader;
    Json::Value rootValue;
    jsonReader.parse(response.getUTF8String(), rootValue);
    accInfo.mAuthor = rootValue["name"].asString();
    auto emails = rootValue["emails"];
    auto email = emails["account"].asString();
    
    accInfo.mAccountDescription = accInfo.mAuthor;
    if(!email.empty())
    {
        accInfo.mAccountDescription += UString("[") + UString(email) + UString("]");
    }
    
    accInfo.mAccountID = rootValue["id"].asString();
    
    accInfo.mTotalSpace = 0;
    
    accInfo.mFreeSpaceLeft = 0;
}


void SkyDriveImpl::RequestAccountInfo(AccInfoCallBackProc_t callBack)
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
    
//    webServiceEngine->SendGetRequest(UString(SkyDrive::kBaseAPIURL) + UString(SkyDrive::kGetAccountInfoURL), params, WebServicesInterface::UserDataType_t());
}

void SkyDriveImpl::ParseMetadata(Json::Value& fileMetadata, FileProperties& fileProp, const UString&folderPath)
{
    fileProp.mFileID = fileMetadata["id"].asString();
    
    if(fileMetadata.isNull())
    {
        //isDeleted = true;
    }
    else
    {
        fileProp.mFileSize = fileMetadata["size"].asUInt64();
        fileProp.mFileName = UString(fileMetadata["name"].asString());
        
        if(!folderPath.empty())
            fileProp.mFilePath = folderPath + UString("/") + fileProp.mFileName;
        
        fileProp.mIsFolder = (fileMetadata["type"] == std::string("folder") || fileMetadata["type"] == std::string("album"));
        
        
        fileProp.mFileRev = fileMetadata["rev"].asString();
        fileProp.mParentID = fileMetadata["parent_id"].asString();
        fileProp.mIsRootElement = fileProp.mParentID == mRootFolderID;
        
        if(fileProp.mIsFolder)
        {
            fileProp.mThumbnail = "folder";
            fileProp.mMimeType = "application/folder";
        }
        else
        {
            auto extension = Utils::GetFileExtension(fileProp.mFilePath);
            fileProp.mMimeType = Utils::GetMimeTypeFromFileExtension(extension);
            Utils::Log(UString("extension:") + extension + UString(", mimetype: ") + fileProp.mMimeType);
            auto lowerExt = extension.lowerCaseString();
            if(lowerExt == L"jpg" || lowerExt == L"jpeg" || lowerExt == L"png" || lowerExt == L"bmp" || lowerExt == L"gif")
            {
                fileProp.mThumbnail = L"";
            }
            else
            {
                fileProp.mThumbnail = Utils::GetIconNameFromExtension(extension);
                if(fileProp.mThumbnail.empty())
                    fileProp.mThumbnail = Utils::GetIconNameFromMimeType(fileProp.mMimeType.getUTF8String());
            }
        }
        
    }

}

void SkyDriveImpl::ListFolderContent(const UString &folderID, const UString &folderPath, FileUpdateCallBackProc_t callBack)
{
    auto isRootItem = (folderPath == UString("/"));
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack, folderID, folderPath, isRootItem](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               Json::Reader jsonReader;
                                                                               Json::Value rootValue;
                                                                               jsonReader.parse(response.getUTF8String(), rootValue);
                                                                               
                                                                               if(mRootFolderID.empty())
                                                                                   mRootFolderID = rootValue["id"].asString();
                                                                               
                                                                               //auto hasMore = rootValue["has_more"].asBool();
                                                                               auto cursor = "1234";
                                                                               auto entries = rootValue["data"];
                                                                               
                                                                               if(entries.size() == 0)
                                                                               {
                                                                                   FileProperties fileProp;
                                                                                   callBack(false, cursor, fileProp);
                                                                               }
                                                                               for(auto fileMetadata:entries)
                                                                               {
                                                                                   bool isDeleted = false;
                                                                                   FileProperties fileProp;
                                                                                   //auto fileMetadata = entry[0];
                                                                                   UString checkPoint;
                                                                                   
                                                                                   ParseMetadata(fileMetadata, fileProp, UString());
                                                                                   
                                                                                   if(fileMetadata.isNull())
                                                                                   {
                                                                                       isDeleted = true;
                                                                                   }
                                                                                   else{
                                                                                       
                                                                                   }
                                                                                   
                                                                                   
                                                                                   if(fileProp.mIsFolder)
                                                                                   {
                                                                                       callBack(isDeleted, L"", fileProp);
                                                                                       ListFolderContent(fileProp.mFileID, fileProp.mFilePath, callBack);
                                                                                   }
                                                                                   else
                                                                                   {
                                                                                       callBack(isDeleted, L"", fileProp);
                                                                                   }
                                                                                   
                                                                                   //++index;
                                                                               }
                                                                               
                                                                           }
                                                                       });
    
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    
    
    webServiceEngine->SendGetRequest(UString(SkyDrive::kBaseAPIURL) + UString("/") + folderID + UString(SkyDrive::kListFolderContentURL), params, WebServicesInterface::UserDataType_t());
    
}

void SkyDriveImpl::StartSendingUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint)
{
    if(checkpoint.empty())
    {
        ListFolderContent(L"me/skydrive", L"/", callBack);
    }
}

void SkyDriveImpl::AuthenticateStage2(PlatformView parentView, AuthCallBackProc_t callBack, const UString &oauthToken, const UString &oauthSecret)
{
    //PlatformView parentView, const UString& url, const UString &callBackUrl, AuthCallBackProc_t callback
    
    UString baseURL(L"https://www.SkyDrive.com/1/oauth/authorize");
    
	auto callbackURI = Utils::GetAppCallbackURL();
    
	WebServicesInterface::ParametersType_t params;
    params[UString(L"oauth_token")] = oauthToken;
    params[UString(L"oauth_callback")] = callbackURI;
    //params[UString(L"display")] = UString(L"mobile");
    
    auto callBackFunc = std::bind(&SkyDriveImpl::HandleAuthStage1Response, this, (PlatformView)NULL, callBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
    /**/
	auto strURL = webServiceEngine->EncodeURL(baseURL, params);
    
//    mOauthDlgPtr.reset(new OAuthDialog(parentView, strURL, callbackURI, [this, callBack, oauthSecret](bool success, const UString& requestToken)->void{
//        if(success && requestToken.size())
//        {
//            UString reqURL = CombineURL(SkyDrive::kBaseAPIURL, SkyDrive::kGetTokenURL);
//            WebServicesInterface::ParametersType_t params;
//            params[UString(L"oauth_consumer_key")] = UString(SkyDrive::kAppBaseKey);
//            params[UString(L"oauth_token")] = requestToken;
//            params[UString(L"oauth_signature_method")] = UString(L"PLAINTEXT");;
//            params[UString(L"oauth_signature")] = UString(SkyDrive::kAppSecret) + L"&" + oauthSecret;
//            params[UString(L"oauth_timestamp")] = Utils::GenerateTimestamp();
//            params[UString(L"oauth_nonce")] = Utils::GenerateUUID();
//            
//            auto callBackFunc = std::bind(&SkyDriveImpl::HandleAuthStage2Response, this, callBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
//            auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
//            webServiceEngine->SendPostRequest(reqURL, BinaryData_t(), params, WebServicesInterface::UserDataType_t());
//        }
//        else
//        {
//            callBack(kAuthenticationDenied, UString(), AccountInfo());
//        }
//        
//    }));
    
}

void SkyDriveImpl::HandleAuthStage1Response(PlatformView parentView, AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
{
    Utils::Log(UString("Code:") + UString(code) + UString(", response: ") + response);
    if(code == kHTTPResponseOK)
    {
        Json::Reader jsonReader;
        Json::Value rootValue;
        jsonReader.parse(response.getUTF8String(), rootValue);
        
        mTokenType = rootValue["token_type"].asString();
        mAccessToken = rootValue["access_token"].asString();
        mRefreshToken = rootValue["refresh_token"].asString();
        
        AuthenticateWithToken(mAccessToken + UString(" ") + mRefreshToken, callBack);
    }
    else
    {
        callBack(kAuthenticationUnknownErr, UString(), AccountInfo());
    }
    Utils::Log("After stage 2");
}

void SkyDriveImpl::HandleAuthStage2Response(AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
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

UString SkyDriveImpl::GetServiceRootFolder() const
{
    auto rootFolder = Utils::GetAppDataFolderPath();
    rootFolder.AppendPathComponent("SkyDrive");
    rootFolder.AppendPathComponent(mUID);    
    return rootFolder;
}

UString SkyDriveImpl::GetThumbnailRootFolder() const
{
    auto rootFolder = GetServiceRootFolder();
    rootFolder.AppendPathComponent("ThumbnailCache");
    return rootFolder;
}

UString SkyDriveImpl::GetDownloadRootFolder() const
{
    auto rootFolder = GetServiceRootFolder();
    rootFolder.AppendPathComponent("DownloadCache");
    return rootFolder;
}

void SkyDriveImpl::DownloadThumbnail(const UString &fileID, const UString &path, int width, int height, ThumbnailCallBackProc_t callback)
{
    
    GeneratePublicURL(fileID, [this, path, width, height, callback](const UString &fileID, const UString &fileURL)
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
                          const char *strSize = "thumbnail";
                          if(maxDim <= 100)
                              strSize = "small";
                          else if(maxDim <= 200)
                              strSize = "album";
                          else 
                              strSize = "normal";
                          
                          params["type"] = strSize;
                          params["url"] = fileURL;
                          
                          webServiceEngine->SendGetRequest(UString(SkyDrive::kBaseAPIURL) + UString(SkyDrive::kThumbnailReqURL), params, WebServicesInterface::UserDataType_t());

                      });
    }


void SkyDriveImpl::DownloadFile(const UString &fileID, const UString &path, FileCallBackProc_t callback)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(WebServicesInterface::CallBackProc_t(), [this, callback, path, fileID](int code, const WebServicesInterface::UserDataType_t& userData, const unsigned char* response, size_t size, const WebServicesInterface::HeaderListType_t& headers)
        {
            if(code == kHTTPResponseOK)
            {
                UString rev = L"123456", actualFileName;
                UString actualFilePath;
                                
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
    
    webServiceEngine->SendGetRequest(UString(SkyDrive::kBaseAPIURL) + fileID + UString(SkyDrive::kFileGetReqURL), params, WebServicesInterface::UserDataType_t());
}

void SkyDriveImpl::UploadFile(const UString &localFilePath, const UString &remoteFilePath, FileUploadCallBackProc_t callback)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, localFilePath, remoteFilePath](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
        {
            if(code == kHTTPResponseOK)
            {
                Utils::Log(UString("Upload complete for ") + localFilePath);
                callback(true, FileProperties());
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
//    webServiceEngine->SendPutRequest(UString(SkyDrive::kBaseContentAPIURL) + UString(SkyDrive::kFilePutReqURL) + UString(SkyDrive::kSkyDriveRoot) + remoteFilePath, fileData, params, WebServicesInterface::UserDataType_t());
}

void SkyDriveImpl::GeneratePublicURL(const UString &fileID, FileURLCallBackProc_t callback)
{
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, fileID](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               Json::Reader jsonReader;
                                                                               Json::Value rootValue;
                                                                               jsonReader.parse(response.getUTF8String(), rootValue);
                                                                               UString sharedURL = rootValue["link"].asString();
                                                                               
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

    webServiceEngine->SendGetRequest(UString(SkyDrive::kBaseAPIURL) + fileID + UString(SkyDrive::kGetPublicURL), params, WebServicesInterface::UserDataType_t());
}

UString SkyDriveImpl::GetIconName(const UTF8String& SkyDriveIcon)
{
    UString appIconId;
        
    auto iconName = mSkyDriveToIconsMapping.find(SkyDriveIcon);
    
    
    if(iconName != mSkyDriveToIconsMapping.end())
    {
        appIconId = iconName->second;
    }
    else
    {
        appIconId = "application-x-zerosize";
        Utils::Log(UString("Could not convert SkyDrive icon: ") + UString(SkyDriveIcon));
    }
    
    return appIconId;
}

void SkyDriveImpl::CopyItem(const UString &srcfileID, const UString &, const UString &destFileLocation, CopyOperationCallbackProc_t callback){
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK){
                                                                               Json::Reader jsonReader;
                                                                               Json::Value rootVal;
                                                                               jsonReader.parse(response.getUTF8String(), rootVal);
                                                                               
                                                                               //callback(true, rootVal["id"].asString());
                                                                           }
                                                                           else{
                                                                               callback(false, FileProperties());
                                                                           }
                                                                       });
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    Json::StyledWriter jsonWriter;
    
    Json::Value fileResource;
    
    fileResource["destination"] = destFileLocation.getUTF8String();
    
    webServiceEngine->SendRequestByMethod("COPY", UString(SkyDrive::kBaseAPIURL) + srcfileID, jsonWriter.write(fileResource), params, WebServicesInterface::UserDataType_t());

}

void SkyDriveImpl::MoveItem(const UString &srcfileID, const UString &, const UString &destFileLocation, MoveOperationCallbackProc_t callback){
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK){
                                                                               Json::Reader jsonReader;
                                                                               Json::Value rootVal;
                                                                               jsonReader.parse(response.getUTF8String(), rootVal);
                                                                               
                                                                               callback(true, rootVal["id"].asString());
                                                                           }
                                                                           else{
                                                                               callback(false, UString());
                                                                           }
                                                                       });
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    Json::StyledWriter jsonWriter;
    
    Json::Value fileResource;
    
    fileResource["destination"] = destFileLocation.getUTF8String();
    
    webServiceEngine->SendRequestByMethod("MOVE", UString(SkyDrive::kBaseAPIURL) + srcfileID, jsonWriter.write(fileResource), params, WebServicesInterface::UserDataType_t());
}

void SkyDriveImpl::RenameItem(const UString &srcfileID, const UString &destFileLocation, const UString& newName, MoveOperationCallbackProc_t callback){
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK){
                                                                               Json::Reader jsonReader;
                                                                               Json::Value rootVal;
                                                                               jsonReader.parse(response.getUTF8String(), rootVal);
                                                                               
                                                                               callback(true, rootVal["id"].asString());
                                                                           }
                                                                           else{
                                                                               callback(false, UString());
                                                                           }
                                                                       });
    WebServicesInterface::ParametersType_t params;
    GetDefaultReqParameters(params);
    
    Json::StyledWriter jsonWriter;
    
    Json::Value fileResource;

    fileResource["name"] = newName.getUTF8String();
    
    webServiceEngine->SendPutRequest(UString(SkyDrive::kBaseAPIURL) + srcfileID, jsonWriter.write(fileResource), params, WebServicesInterface::UserDataType_t());
    
}

void SkyDriveImpl::DeleteItem(const UString &fileID, FileOperationCallbackProc_t callback){
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
    
    webServiceEngine->SendRequestByMethod("DELETE", UString(SkyDrive::kBaseAPIURL) + fileID, BinaryData_t(), params, WebServicesInterface::UserDataType_t());
}

void SkyDriveImpl::CreateFolder(const UString& destFileId, const UString &folderName, FileOperationCallbackProc_t callback){
    /*auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, folderPathPath](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
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
    
//    WebServicesInterface::ParametersType_t params;
//    GetDefaultReqParameters(params);
//    params["root"] = SkyDrive::kSkyDriveString;
//    params["path"] = folderPathPath;
//    
//    webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
//    webServiceEngine->SendPostRequest(UString(SkyDrive::kBaseAPIURL) + UString(SkyDrive::kCreateFolderURL), BinaryData_t(), params, WebServicesInterface::UserDataType_t());*/
}
