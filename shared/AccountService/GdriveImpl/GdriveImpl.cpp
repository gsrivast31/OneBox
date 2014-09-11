//
//  GdriveImpl.cpp
//  AccountServices
//
//  Created by Vikash Jindal on 04/28/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#include "GdriveImpl.h"
#include "CommonUtils.h"
#include "StringConstants.h"
#include "ODLogger.h"
#include <json.h>
#include <memory>
#include "AccountUtils.h"

using namespace NewGroup::Core;
std::map<UTF8String, UTF8String> GdriveImpl::mGDriveToIconsMapping;
std::map<UTF8String, UString> GdriveImpl::mFolderIdtoTitleMapping;

std::set<UTF8String> GdriveImpl::mAvailableIcons;

void GdriveImpl::Authenticate(PlatformView parentView, AuthCallBackProc_t callBack)
{
    auto callBackFunc = std::bind(&GdriveImpl::HandleAuthStage1Response, this, callBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance(callBackFunc);
    
    WebServicesInterface::ParametersType_t params;
	params[UString(L"scope")] = UString(L"https://www.googleapis.com/auth/drive https://www.googleapis.com/auth/userinfo.email");
    params[UString(L"redirect_uri")] = UString(L"http://localhost:8080");
    params[UString(L"response_type")] = UString("code");
	params[UString(L"client_id")] = UString(GDrive::kClient_id);
    params[UString(L"state")] = UString(L"security_token");
    params[UString(L"approval_prompt")] = UString(L"force");
    UString baseURL(L"https://accounts.google.com/o/oauth2/auth");
    auto strURL = webServiceEngine->EncodeURL(baseURL, params);
	auto callbackURI = UString(L"http://localhost:8080");
    
	UString oauthSecret;
    UString token(L"code");
    mOauthDlgPtr.reset(new OAuthDialog(parentView, strURL, callbackURI, token, [this, webServiceEngine, oauthSecret, callBack](bool success, const UString& requestToken)->void{
        if(success && requestToken.size())
        {
            
            UString reqURL = L"https://accounts.google.com/o/oauth2/token";
            WebServicesInterface::ParametersType_t params, noparam;
            params[UString(L"code")] = requestToken;
            params[UString(L"redirect_uri")] = UString(L"http://localhost:8080");
            params[UString(L"scope")] = UString(L"https://www.googleapis.com/auth/drive https://www.googleapis.com/auth/userinfo.email");
            params[UString(L"grant_type")] = UString(L"authorization_code");
            params[UString(L"client_id")] = UString(GDrive::kClient_id);
            params[UString(L"client_secret")] = UString(GDrive::kClientSecret);
            UString binarydata = webServiceEngine->EncodeURL(UString(L""), params);
            webServiceEngine->SetRequestHeader("Content-Type", UString("application/x-www-form-urlencoded"));
            webServiceEngine->SetPriority(kTaskPriority_High);
            webServiceEngine->SendPostRequest(reqURL, binarydata, noparam, WebServicesInterface::UserDataType_t());
            
        }
        else
        {
            callBack(kAuthenticationDenied, UString(), AccountInfo());
        }
        
    }));
    
    
}

GdriveImpl::GdriveImpl()
{
    mSessionManager.reset(new SessionManager(UString(), 0, std::bind(&GdriveImpl::RefreshToken, this, std::placeholders::_1)));
    if(mAvailableIcons.empty())
    {
        mAvailableIcons.insert("application-illustrator");
        mAvailableIcons.insert("application-javascript");
        mAvailableIcons.insert("application-ms-access");
        mAvailableIcons.insert("application-ms-excel");
        mAvailableIcons.insert("application-ms-powerpoint");
        mAvailableIcons.insert("application-msword");
        mAvailableIcons.insert("application-pdf");
        mAvailableIcons.insert("application-vnd.ms-access");
        mAvailableIcons.insert("application-vnd.ms-excel");
        mAvailableIcons.insert("application-vnd.ms-powerpoint");
        mAvailableIcons.insert("application-x-archive");
        mAvailableIcons.insert("application-x-cd-image");
        mAvailableIcons.insert("application-x-gzip");
        mAvailableIcons.insert("application-x-php");
        mAvailableIcons.insert("application-x-ruby");
        mAvailableIcons.insert("application-x-zerosize");
        mAvailableIcons.insert("application-xml");
        mAvailableIcons.insert("applications-graphics-2");
        mAvailableIcons.insert("audio-x-generic");
        mAvailableIcons.insert("folder");
        mAvailableIcons.insert("image-svg+xml");
        mAvailableIcons.insert("image-x-generic");
        mAvailableIcons.insert("text-plain");
        mAvailableIcons.insert("text-x-c++hdr");
        mAvailableIcons.insert("text-x-c++src");
        mAvailableIcons.insert("text-x-csharp");
        mAvailableIcons.insert("text-x-csrc");
        mAvailableIcons.insert("text-x-java");
        mAvailableIcons.insert("text-x-makefile");
        mAvailableIcons.insert("text-x-objchdr");
        mAvailableIcons.insert("text-x-python");
        mAvailableIcons.insert("text-x-script");
        mAvailableIcons.insert("unknown");
        mAvailableIcons.insert("video-x-generic");
    }
    
    if(mGDriveToIconsMapping.empty())
    {
        
        mGDriveToIconsMapping["application-vnd.google-apps.audio"] = "audio-x-generic";
        mGDriveToIconsMapping["application-vnd.google-apps.document"] = "application-msword";
        //mGDriveToIconsMapping["application-vnd.google-apps.drawing"] =
        mGDriveToIconsMapping["application-vnd.google-apps.file"] = "application-x-zerosize";
        mGDriveToIconsMapping["application-vnd.google-apps.folder"] = "folder";
        mGDriveToIconsMapping["application-vnd.google-apps.form"] =  "application-x-zerosize";
        mGDriveToIconsMapping["application-vnd.google-apps.fusiontable"] =
        mGDriveToIconsMapping["application-vnd.google-apps.kix"] = "application-msword";
        //mGDriveToIconsMapping["application-vnd.google-apps.photo"] =
        mGDriveToIconsMapping["application-vnd.google-apps.presentation"] = "application-ms-powerpoint";
        mGDriveToIconsMapping["application-vnd.google-apps.script"] = "text-x-script";
        mGDriveToIconsMapping["application-vnd.google-apps.sites"] =  "application-x-zerosize";
        mGDriveToIconsMapping["application-vnd.google-apps.spreadsheet"] = "application-ms-excel";
        mGDriveToIconsMapping["application-vnd.google-apps.unknown"] = "unknown";
        mGDriveToIconsMapping["application-vnd.google-apps.video"] = "video-x-generic";
        
    }
}

GdriveImpl::~GdriveImpl()
{
    
}

void GdriveImpl::RefreshToken(RefreshTokenCallbackProc_t callBack)
{
    Utils::Log(UString("Refreshing Access Token: ") + mRefreshToken);
    auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                       {
                                                                           if(code == kHTTPResponseOK)
                                                                           {
                                                                               UString accessToken;
                                                                               ParseToken(response, accessToken);
                                                                               callBack(accessToken, mExpiresIn, 0);
                                                                               
                                                                           }
                                                                           else if(code == kHTTPTokenExpired)
                                                                           {
                                                                               callBack(UString(), 0, kHTTPTokenExpired);
                                                                           }
                                                                           else
                                                                           {
                                                                               callBack(UString(), 0, kHTTPUnknownError);
                                                                           }
                                                                       });
    
    UString reqURL = L"https://accounts.google.com/o/oauth2/token";
    WebServicesInterface::ParametersType_t params, noparam;
    params[UString(L"grant_type")] = UString(L"refresh_token");
    params[UString(L"client_id")] = UString(GDrive::kClient_id);
    params[UString(L"client_secret")] = UString(GDrive::kClientSecret);
    params[UString(L"refresh_token")] = mRefreshToken;
    params[UString(L"scope")] = UString(L"https://www.googleapis.com/auth/drive https://www.googleapis.com/auth/userinfo.email");
    
    UString binarydata = webServiceEngine->EncodeURL(UString(L""), params);
    webServiceEngine->SetRequestHeader("Content-Type", UString("application/x-www-form-urlencoded"));
    webServiceEngine->SetPriority(kTaskPriority_High);
    webServiceEngine->SendPostRequest(reqURL, binarydata, noparam, WebServicesInterface::UserDataType_t());
}

void GdriveImpl::AuthenticateWithToken(const UString& authToken, AuthCallBackProc_t callBack)
{
    mRefreshToken = authToken;
    GetUserInfo(callBack);
}

void GdriveImpl::ParseAccountInfo(const UString& response, AccountInfo& accInfo)
{
    Json::Reader jsonReader;
    Json::Value rootValue;
    jsonReader.parse(response.getUTF8String(), rootValue);
    
    auto entry = rootValue["user"];
    accInfo.mAuthor = entry["displayName"].asString();
    auto desc = accInfo.mAccountDescription;
    accInfo.mAccountDescription = accInfo.mAuthor;
    accInfo.mAccountDescription += desc;
    accInfo.mTotalSpace = atol(rootValue["quotaBytesTotal"].asString().c_str());
    auto quotaBytesUsed = atol(rootValue["quotaBytesUsed"].asString().c_str());
    accInfo.mFreeSpaceLeft = accInfo.mTotalSpace - quotaBytesUsed;
    mRootFolderId = rootValue["rootFolderId"].asString();
    mFolderIdtoTitleMapping[mRootFolderId] = "";
}


void GdriveImpl::RequestAccountInfo(AccInfoCallBackProc_t callBack)
{
    mSessionManager->GetAccessToken([this, callBack](const UString& accessToken){
        UString baseURL(L"https://www.googleapis.com/oauth2/v1/userinfo");
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   UString email = rootValue["email"].asString();
                                                                                   UString id = rootValue["id"].asString();
                                                                                   GetAboutInfo(callBack, email, id);
                                                                               }
                                                                           });
        
        Utils::Log(UString("Authenticating with Access Token: ") + mTokenType + UString(" ") + accessToken);
        webServiceEngine->SetPriority(kTaskPriority_High);
        webServiceEngine->SendGetRequest(baseURL, params, WebServicesInterface::UserDataType_t());
    });
    
}

UString GdriveImpl::GenerateParent(Json::Value &entries, UTF8String &id)
{
    auto it = mFolderIdtoTitleMapping.find(id);
    if(it != mFolderIdtoTitleMapping.end())
    {
        return it->second;
    }
    else
    {
        for(auto entry:entries)
        {
            if(entry["fileId"].asString() == id)
            {
                auto file = entry["file"];
                auto path = file["title"].asString();
                auto parents = file["parents"];
                auto parentid = parents[0]["id"].asString();
                return GenerateParent(entries, parentid) + UString("/") + UString(path);
            }
        }
        return "/";
    }
}

void GdriveImpl::StartSendingUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint)
{
    mPendingChildren = 0;
    RequestUpdates(callBack, checkpoint, UString());
}

bool GdriveImpl::ParseMetadata(Json::Value& file, const UString& path, FileProperties& fileProp)
{
   fileProp.mFileID = file["id"].asString();
    auto isDeleted = file["deleted"].asBool();
    auto isTrashed = file["labels"]["trashed"].asBool();
    
    if(!isDeleted || !isTrashed)
    {
        fileProp.mFileSize = atol(file["quotaBytesUsed"].asString().c_str());
        fileProp.mFileName = file["title"].asString();
        fileProp.mFilePath = path + UString("/") + fileProp.mFileName;
        
        fileProp.mFileRev = file["etag"].asString();
        fileProp.mMimeType = file["mimeType"].asString();
        if(file["mimeType"].asString() == std::string("application/vnd.google-apps.folder"))
        {
            fileProp.mIsFolder = true;
        }
        else
            fileProp.mIsFolder = false;
        fileProp.mLastModifiedTime = DateTime(UString(file["modifiedDate"].asString()), UString(GDrive::kDateTimeFormat));
        
        fileProp.mThumbnail = GetIconName(fileProp.mMimeType.getUTF8String());
        auto parents = file["parents"];
        auto parentid = parents[0]["id"].asString();
        
        fileProp.mIsRootElement = (parents.empty() || parents[0]["isRoot"].asBool());
        
        fileProp.mParentID = parentid;
        
        return false;
        
    }
    
    return true;

}

void GdriveImpl::RequestMetadata(const UString& fileID, const UString& path, const UString& checkpoint, FileUpdateCallBackProc_t callBack)
{
    mSessionManager->GetAccessToken([this, fileID, path, callBack, checkpoint](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, fileID, path, callBack, checkpoint](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   FileProperties fileProp;
                                                                                   
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   
                                                                                   bool deleted = ParseMetadata(rootValue, path, fileProp);
                                                                                   
                                                                                   callBack(deleted, L"", fileProp);
                                                                                   
                                                                                   if(!deleted && fileProp.mIsFolder)
                                                                                   {
                                                                                       GetChildrenList(fileProp.mFileID, fileProp.mFilePath, UString(), checkpoint, callBack);
                                                                                   }
                                                                               }
                                                                               
                                                                               if((mPendingChildren--) == 1)
                                                                               {
                                                                                   callBack(false, checkpoint, FileProperties());
                                                                               }
                                                                           });
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        
        mPendingChildren++;
        webServiceEngine->SetPriority(kTaskPriority_Low);
        webServiceEngine->SendGetRequest(UString(GDrive::kFileOpsURL) + fileID, params, WebServicesInterface::UserDataType_t());
    });
}

void GdriveImpl::GetChildrenList(const UString& root, const UString& path, const UString& pageToken, const UString& checkpoint, FileUpdateCallBackProc_t callBack)
{
    mSessionManager->GetAccessToken([this, pageToken, root, path, callBack, checkpoint](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, pageToken, root, path, callBack, checkpoint](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   
                                                                                   auto children = rootValue["items"];
                                                                                   for(auto child:children)
                                                                                   {
                                                                                       auto childID = child["id"].asString();
                                                                                       RequestMetadata(childID, path, checkpoint, callBack);
                                                                                   }
                                                                                   
                                                                                   auto nextPageToken = rootValue["nextPageToken"].asString();
                                                                                   
                                                                                   if(!nextPageToken.empty())
                                                                                   {
                                                                                       GetChildrenList(root, path, pageToken, checkpoint, callBack);
                                                                                   }
                                                                                   
                                                                                   
                                                                               }
                                                                               
                                                                               if((mPendingChildren--) == 1)
                                                                               {
                                                                                   callBack(false, checkpoint, FileProperties());
                                                                               }
                                                                               
                                                                           });
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        if(!pageToken.empty())
            params[UString(L"pageToken")] =  pageToken;
        webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
        mPendingChildren++;
        webServiceEngine->SetPriority(kTaskPriority_Low);
        webServiceEngine->SendGetRequest(UString(GDrive::kFileOpsURL) + root + UString(GDrive::kChildrenURL), params, WebServicesInterface::UserDataType_t());

    });
}

void GdriveImpl::RequestUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint, const UString& pageToken)
{
    mSessionManager->GetAccessToken([this, callBack, checkpoint, pageToken](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack, checkpoint](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   
                                                                                   auto largestChangeId = rootValue["largestChangeId"].asString();
                                                                                   auto nextStartingChangeId = UString::fromValue(atol(largestChangeId.c_str()) + 1);
                                                                                   bool hasMore = true;
                                                                                   auto entries = rootValue["items"];
                                                                                   auto nextPageToken = rootValue["nextPageToken"].asString();
                                                                                   auto index = 1;
                                                                                   if(entries.size() == 0)
                                                                                   {
                                                                                       FileProperties fileProp;
                                                                                       callBack(false, nextStartingChangeId, fileProp);
                                                                                       hasMore = false;
                                                                                   }
                                                                                   
                                                                                   if(checkpoint.empty())
                                                                                   {
                                                                                       //Send data hierarchy
                                                                                       GetChildrenList(L"root", L"", L"", nextStartingChangeId, callBack);
                                                                                       return;
                                                                                   }
                                                                                   for(auto entry:entries)
                                                                                   {
                                                                                       bool isDeleted = false;
                                                                                       FileProperties fileProp;
                                                                                       
                                                                                       fileProp.mFileID = entry["fileId"].asString();
                                                                                       if(nextPageToken.empty())
                                                                                           hasMore = false;
                                                                                       auto file = entry["file"];
                                                                                       isDeleted = ParseMetadata(file, UString(), fileProp
                                                                                                                 );
                                                                                       callBack(isDeleted, ((entries.size() == index) && !hasMore)? nextStartingChangeId : "", fileProp);
                                                                                       
                                                                                       ++index;
                                                                                   }
                                                                                   
                                                                                   if(hasMore)
                                                                                   {
                                                                                       RequestUpdates(callBack, checkpoint, nextPageToken);
                                                                                   }
                                                                               }
                                                                           });
        
        WebServicesInterface::ParametersType_t params;
        params[L"includeDeleted"] = UString("false");
        
        if(!checkpoint.empty())
        {
            params[L"startChangeId"] = checkpoint;
            params[L"includeDeleted"] = UString("true");
            
        }
        
        if(!pageToken.empty())
        {
            params[L"pageToken"] = pageToken;
        }
        
        params[UString(L"includeSubscribed")] = UString("false");
        params[UString(L"access_token")] =  accessToken;
        //params[UString(L"maxResults")] = UString(1000);
        WebServicesInterface::UserDataType_t userData;
        webServiceEngine->SetPriority(kTaskPriority_Low);
        
        webServiceEngine->SendGetRequest(UString(L"https://www.googleapis.com/drive/v2/changes"), params, userData);
    });
}

void GdriveImpl::GetAboutInfo(AccInfoCallBackProc_t callBack, UString email, UString id)
{
    mSessionManager->GetAccessToken([this, callBack, email, id](const UString& accessToken){
        UString baseURL(L"https://www.googleapis.com/drive/v2/about");
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack, email, id](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   AccountInfo accInfo;
                                                                                   accInfo.mAccountID = id;
                                                                                   accInfo.mAccountDescription = UString("[") + UString(email) + UString("]");
                                                                                   ParseAccountInfo(response, accInfo);
                                                                                   accInfo.mAccountDescription = accInfo.mAuthor;
                                                                                   callBack(accInfo);
                                                                                   
                                                                               }
                                                                           });
        webServiceEngine->SetPriority(kTaskPriority_High);
        webServiceEngine->SendGetRequest(baseURL, params, WebServicesInterface::UserDataType_t());
    });
    
}

void GdriveImpl::GetAboutInfo(AuthCallBackProc_t callBack, UString email, UString id)
{
    mSessionManager->GetAccessToken([this, callBack, email, id](const UString& accessToken){
        UString baseURL(L"https://www.googleapis.com/drive/v2/about");
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack, email, id](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   AccountInfo accInfo;
                                                                                   accInfo.mAccountID = id;
                                                                                   accInfo.mAccountDescription = UString("[") + UString(email) + UString("]");
                                                                                   ParseAccountInfo(response, accInfo);
                                                                                   accInfo.mAccountDescription = accInfo.mAuthor;
                                                                                   callBack(kAuthenticationPassed, mRefreshToken , accInfo);
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
        webServiceEngine->SetPriority(kTaskPriority_High);
        webServiceEngine->SendGetRequest(baseURL, params, WebServicesInterface::UserDataType_t());
    });
}

void GdriveImpl::GetUserInfo(AuthCallBackProc_t callBack)
{
    
    mSessionManager->GetAccessToken([this, callBack](const UString& accessToken){
        UString baseURL(L"https://www.googleapis.com/oauth2/v1/userinfo");
        WebServicesInterface::ParametersType_t params;
        
        params[UString(L"access_token")] =  accessToken;
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callBack](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   UString email = rootValue["email"].asString();
                                                                                   UString id = rootValue["id"].asString();
                                                                                   GetAboutInfo(callBack, email, id);
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
        webServiceEngine->SetPriority(kTaskPriority_High);
        webServiceEngine->SendGetRequest(baseURL, params, WebServicesInterface::UserDataType_t());

    });
        
}
void GdriveImpl::ParseToken(const UString& response, UString& accessToken)
{
    Json::Reader jsonReader;
    Json::Value rootValue;
    jsonReader.parse(response.getUTF8String(), rootValue);
    
    accessToken = rootValue["access_token"].asString();
    mRefreshToken = rootValue["refresh_token"].asString();
    mExpiresIn = rootValue["expires_in"].asInt();
    mTokenType = rootValue["token_type"].asString();
    mIdToken = rootValue["id_token"].asString();
    
    //mSessionManager.ManageToken(mExpiresIn, SharedSessionCallBackProc_t());
    //Utils::Log(UString("mAccessToken:") + mAccessToken+ UString("\nrefresh_token:") + mRefreshToken + UString("\nmExpiresIn: ") + mExpiresIn + UString("\nmTokenType:") + mTokenType + UString("\nmIdToken: ") + mIdToken);
    
    
}
void GdriveImpl::HandleAuthStage1Response(AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
{
    Utils::Log(UString("Code:") + UString(code) + UString(", response: ") + response);
    if(code == kHTTPResponseOK)
    {
        std::map<UString, UString> paramKeyValues;
        if(code == kHTTPResponseOK)
        {
            UString accessToken;
            ParseToken(response, accessToken);
            mSessionManager.reset(new SessionManager(accessToken, mExpiresIn, std::bind(&GdriveImpl::RefreshToken, this, std::placeholders::_1)));
            GetUserInfo(callBack);
        }
    }
    else
    {
        callBack(kAuthenticationUnknownErr, UString(), AccountInfo());
    }
    Utils::Log("After stage 2");
}

UString GdriveImpl::GetServiceRootFolder() const
{
    auto rootFolder = Utils::GetAppDataFolderPath();
    rootFolder.AppendPathComponent("GDrive");
    rootFolder.AppendPathComponent(mUID);
    return rootFolder;
}

UString GdriveImpl::GetThumbnailRootFolder() const
{
    auto rootFolder = GetServiceRootFolder();
    rootFolder.AppendPathComponent("ThumbnailCache");
    return rootFolder;
}

UString GdriveImpl::GetDownloadRootFolder() const
{
    auto rootFolder = GetServiceRootFolder();
    rootFolder.AppendPathComponent("DownloadCache");
    return rootFolder;
}


void GdriveImpl::DownloadThumbnail(const UString &fileID, const UString &path, int width, int height, ThumbnailCallBackProc_t callback)
{
    mSessionManager->GetAccessToken([this, callback, fileID, width, height, path](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, accessToken, callback, fileID, width, height, path](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   auto url = rootValue["thumbnailLink"].asString();
                                                                                   auto title = rootValue["title"].asString();
                                                                                   
                                                                                   //Utils::Log(UString(url));
                                                                                   //Utils::Log(UString(filePath));
                                                                                   auto webServiceEngine = WebServicesInterface::CreateEngineInstance(WebServicesInterface::CallBackProc_t(), [this, callback, title, fileID, width, height, path, accessToken](int code, const WebServicesInterface::UserDataType_t& userData, const unsigned char* response, size_t size, const WebServicesInterface::HeaderListType_t& headers)
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
                                                                                   webServiceEngine->SetRequestHeader("Authorization", UString(" ") + mTokenType + UString(" ") + accessToken);
                                                                                   WebServicesInterface::UserDataType_t userData;
                                                                                   webServiceEngine->SendGetRequest(url, params, WebServicesInterface::UserDataType_t());
                                                                                   
                                                                               }
                                                                           });
        
        
        WebServicesInterface::ParametersType_t params;
        Utils::Log(UString("FileId: ") + fileID);
        //params[UString(L"fileId")] = fileID;
        params[UString(L"access_token")] =  accessToken;
        WebServicesInterface::UserDataType_t userData;
        webServiceEngine->SetPriority(kTaskPriority_Low);
        webServiceEngine->SendGetRequest(UString("https://www.googleapis.com/drive/v2/files/")+fileID, params, WebServicesInterface::UserDataType_t());

    });
    
}



void GdriveImpl::DownloadFile(const UString &fileID, const UString &path, FileCallBackProc_t callback)
{
    mSessionManager->GetAccessToken([this, callback, fileID, path](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, fileID, path, accessToken](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   auto url = rootValue["downloadUrl"].asString();
                                                                                   //auto title = rootValue["title"].asString();
                                                                                   auto etag = rootValue["etag"].asString();
                                                                                   UTF8String exportFormat;
                                                                                   if(url.empty())
                                                                                   {
                                                                                       //fetch exportLinks
                                                                                       auto exportLink = rootValue["exportLinks"];
                                                                                       if(exportLink.size())
                                                                                       {
                                                                                           Json::ValueIterator itr = exportLink.begin();
                                                                                           auto key = itr.key().asString();
                                                                                           url = exportLink[key].asString();
                                                                                           auto index = url.rfind("exportFormat=");
                                                                                           exportFormat = url.substr(index + strlen("exportFormat="));
                                                                                           Utils::Log(UString(exportFormat));
                                                                                       }
                                                                                   }
                                                                                   //Utils::Log(UString(url));
                                                                                   //Utils::Log(UString(filePath));
                                                                                   auto webServiceEngine = WebServicesInterface::CreateEngineInstance(WebServicesInterface::CallBackProc_t(), [this, callback, path, fileID, etag, exportFormat, accessToken](int code, const WebServicesInterface::UserDataType_t& userData, const unsigned char* response, size_t size, const WebServicesInterface::HeaderListType_t& headers)
                                                                                                                                                      {
                                                                                                                                                          if(code == kHTTPResponseOK)
                                                                                                                                                          {
                                                                                                                                                              UString actualFilePath, actualFileName;
                                                                                                                                                              
                                                                                                                                                              Utils::SplitFilePathAndName(path, actualFilePath, actualFileName);
                                                                                                                                                              auto filePath = GetDownloadRootFolder() + actualFilePath;
                                                                                                                                                              Utils::CreateFoldersOfPath(filePath);
                                                                                                                                                              filePath += Utils::GenerateUUID() + L"_" + actualFileName;
                                                                                                                                                              if(!exportFormat.empty())
                                                                                                                                                                  filePath += L"." + UString(exportFormat);
                                                                                                                                                              Utils::CreateFileWithData(filePath, response, size);
                                                                                                                                                              callback(fileID, filePath, etag);
                                                                                                                                                              Utils::Log(UString("Downloaded file ") + path + UString(" saved to ") + filePath);
                                                                                                                                                              
                                                                                                                                                          }
                                                                                                                                                      });
                                                                                   WebServicesInterface::ParametersType_t params;
                                                                                   webServiceEngine->SetRequestHeader("Authorization", UString(" ") + mTokenType + UString(" ") + accessToken);
                                                                                   WebServicesInterface::UserDataType_t userData;
                                                                                   webServiceEngine->SendGetRequest(url, params, WebServicesInterface::UserDataType_t());
                                                                                   
                                                                               }
                                                                           });
        
        
        WebServicesInterface::ParametersType_t params;
        Utils::Log(UString("FileId: ") + fileID);
        //params[UString(L"fileId")] = fileID;
        params[UString(L"access_token")] =  accessToken;
        WebServicesInterface::UserDataType_t userData;
        
        webServiceEngine->SendGetRequest(UString("https://www.googleapis.com/drive/v2/files/")+fileID, params, WebServicesInterface::UserDataType_t());

    });
    
}


void GdriveImpl::UploadFile(const UString &localFilePath, const UString &parentID, FileUploadCallBackProc_t callback)
{
    auto extension = Utils::GetFileExtension(localFilePath);
    auto mimeType = Utils::GetMimeTypeFromFileExtension(extension);
    
    std::vector<UString> pathComponents;
    localFilePath.Split(pathComponents, L"/");
    
    if(pathComponents.empty())
    {
        assert(false);
        callback(false, FileProperties());
        return;
    }
    
    auto fileName = pathComponents[pathComponents.size() - 1];
    
    mSessionManager->GetAccessToken([this, callback, localFilePath, parentID, mimeType, fileName](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, localFilePath, parentID, mimeType, fileName](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   FileProperties fileProps;
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   
                                                                                   ParseMetadata(rootValue, UString(), fileProps);
                                                                                   
                                                                                   Utils::Log(UString("Upload complete for ") + localFilePath);
                                                                                   CreateMetadataEntry(fileProps.mFileID, parentID, mimeType, fileName, [this, callback, localFilePath, parentID](int code, const UString& response){
                                                                                       if(code == kHTTPResponseOK)
                                                                                       {
                                                                                           FileProperties fileProps;
                                                                                           Json::Reader jsonReader;
                                                                                           Json::Value rootValue;
                                                                                           jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                           
                                                                                           ParseMetadata(rootValue, UString(), fileProps);
                                                                                           callback(true, fileProps);
                                                                                       }
                                                                                       else
                                                                                       {
                                                                                           Utils::Log(UString("Upload Metadata for ") + localFilePath + UString(" failed!. Error: ") + UString(code));
                                                                                           callback(false, FileProperties());
                                                                                       }
                                                                                   });
                                                                               }
                                                                               else
                                                                               {
                                                                                   Utils::Log(UString("Upload file data for ") + localFilePath + UString(" failed!. Error: ") + UString(code));
                                                                                   callback(false, FileProperties());
                                                                               }
                                                                           });
        
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        params[UString(L"uploadType")] =  L"media";
        
        auto fileData = Utils::ReadFileData(localFilePath);
        webServiceEngine->SetRequestHeader("Content-Type", mimeType);
        webServiceEngine->SendPostRequest(UString(GDrive::kFileUploadURL)/* + fileProps.mFileID*/, fileData, params, WebServicesInterface::UserDataType_t());
        
    });

    
    
                        
    
    
}

void GdriveImpl::GeneratePublicURL(const UString &fileID, FileURLCallBackProc_t callback)
{
    mSessionManager->GetAccessToken([this, fileID, callback](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, fileID, callback](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   Json::Reader jsonReader;
                                                                                   Json::Value rootValue;
                                                                                   FileProperties fileProp;
                                                                                   
                                                                                   jsonReader.parse(response.getUTF8String(), rootValue);
                                                                                   
                                                                                   auto link = rootValue["alternateLink"].asString();
                                                                                   if(link.empty())
                                                                                       link = rootValue["webViewLink"].asString();
                                                                                   if(link.empty())
                                                                                       link = rootValue["webContentLink"].asString();
                                                                                   if(link.empty())
                                                                                       link = rootValue["embedLink"].asString();
                                                                                   
                                                                                   auto isShared = rootValue["shared"].asBool();
                                                                                   
                                                                                   if(isShared)
                                                                                       callback(fileID, link);
                                                                                   else
                                                                                   {
                                                                                       mSessionManager->GetAccessToken([this, callback, fileID, link](const UString& accessToken){
                                                                                           auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, fileID, link](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                                                                                                              {
                                                                                                                                                                  if(code == kHTTPResponseOK)
                                                                                                                                                                  {
                                                                                                                                                                      callback(fileID, link);
                                                                                                                                                                  }
                                                                                                                                                                  else
                                                                                                                                                                  {
                                                                                                                                                                      Utils::Log(UString("Setting permissions for ") + fileID + UString(" failed!. Error: ") + UString(code));
                                                                                                                                                                      callback(fileID, UString());
                                                                                                                                                                  }
                                                                                                                                                              });
                                                                                           
                                                                                           WebServicesInterface::ParametersType_t params;
                                                                                           params["sendNotificationEmails"] = "false";
                                                                                           params[UString(L"access_token")] =  accessToken;
                                                                                           
                                                                                           Json::StyledWriter jsonWriter;
                                                                                           
                                                                                           Json::Value permissionResource;
                                                                                           
                                                                                           permissionResource["role"] = "reader";
                                                                                           permissionResource["type"] = "anyone";
                                                                                           permissionResource["value"] = "me";
                                                                                           //permissionResource["withLink"] = true;
                                                                                           
                                                                                           auto reqBody = jsonWriter.write(permissionResource);
                                                                                           
                                                                                           webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
                                                                                           webServiceEngine->SendPostRequest(UString(GDrive::kFileOpsURL) + fileID + UString(GDrive::kPermissionsURL), reqBody, params, WebServicesInterface::UserDataType_t());
                                                                                           
                                                                                       });

                                                                                   }
                                                                                   
                                                                                   
                                                                               }
                                                                               else
                                                                               {
                                                                                   callback(fileID, UString());
                                                                               }
                                                                               
                                                                           });
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        
        webServiceEngine->SendGetRequest(UString(GDrive::kFileOpsURL) + fileID, params, WebServicesInterface::UserDataType_t());
    });

    
        
}

UString GdriveImpl::GetIconName(const UTF8String& GdriveIcon)
{
    UString appIconId;
    
    if(GdriveIcon.find("video") !=std::string::npos)
    {
        appIconId = "video-x-generic";
    }
    else if(GdriveIcon.find("audio") !=std::string::npos)
    {
        appIconId = "audio-x-generic";
    }
    else
    {
        size_t pos = GdriveIcon.find("/",0);
        
        if(pos != std::string::npos)
        {
            UTF8String tmp = GdriveIcon;
            tmp.replace(pos, 1, "-");
            appIconId = tmp;
        }
        Utils::Log(UString("GetIconName IN-> ") + appIconId );
        if(mAvailableIcons.find(appIconId.getUTF8String()) == mAvailableIcons.end())
        {
            auto icon = mGDriveToIconsMapping.find(appIconId.getUTF8String());
            if(icon != mGDriveToIconsMapping.end())
            {
                appIconId = icon->second;
            }
            else if(appIconId.getUTF8String().find("image") !=std::string::npos)
            {
                appIconId = "";
            }
            else
                appIconId = "application-x-zerosize";
        }
        
        //        else
        //            appIconId = "application-x-zerosize";
    }
    
    //    auto iconName = mGDriveToIconsMapping.find(dropboxIcon);
    //
    //
    //    if(iconName != mGDriveToIconsMapping.end())
    //    {
    //        appIconId = iconName->second;
    //    }
    //    else
    //    {
    //        appIconId = "application-x-zerosize";
    //        Utils::Log(UString("Could not convert dropbox icon: ") + UString(dropboxIcon));
    //    }
    //
    Utils::Log(UString("GetIconName OUT-> ") + appIconId );
    
    return appIconId;
}

void GdriveImpl::CopyItem(const UString &srcfileID, const UString &, const UString &destFileLocation, CopyOperationCallbackProc_t callback){
    mSessionManager->GetAccessToken([this, srcfileID, destFileLocation, callback](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, srcfileID](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK)
                                                                               {
                                                                                   
                                                                                   callback(true, FileProperties());
                                                                               }
                                                                               else
                                                                               {
                                                                                   Utils::Log(UString("Copy operation for ") + srcfileID + UString(" failed!. Error: ") + UString(code));
                                                                                   callback(false, FileProperties());
                                                                               }
                                                                           });
        
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        
        Json::StyledWriter jsonWriter;
        
        Json::Value fileResource;
        //fileResource["kind"] = "drive#file";
        
        Json::Value parents;
        parents[0] = destFileLocation.getUTF8String();
        fileResource["parents"] = parents;
        auto reqBody = jsonWriter.write(fileResource);
        webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
        webServiceEngine->SendPostRequest(UString(GDrive::kFileOpsURL) + srcfileID + UString(GDrive::kFileCopyURL), reqBody, params, WebServicesInterface::UserDataType_t());
    });
}

void GdriveImpl::MoveItem(const UString &srcfileID, const UString &, const UString &destFileLocation, MoveOperationCallbackProc_t callback){
    
    CreateMetadataEntry(srcfileID, destFileLocation, UString(), UString(), [this, callback](int code, const UString& response){
        if(code == kHTTPResponseOK){
            Json::Reader jsonReader;
            Json::Value rootValue;
            FileProperties fileProp;
            
            jsonReader.parse(response.getUTF8String(), rootValue);
            
            ParseMetadata(rootValue, UString(), fileProp);
            callback(true, fileProp.mFileID);
        }
        else{
            callback(false, UString());
        }
    });
}

void GdriveImpl::RenameItem(const UString &srcfileID, const UString &destFileLocation, const UString& newName, MoveOperationCallbackProc_t callback){
    CreateMetadataEntry(srcfileID, UString(), UString(), newName, [this, callback, newName](int code, const UString& response){
        if(code == kHTTPResponseOK){
            Json::Reader jsonReader;
            Json::Value rootValue;
            FileProperties fileProp;
            
            jsonReader.parse(response.getUTF8String(), rootValue);
            
            ParseMetadata(rootValue, UString(), fileProp);
            callback(true, fileProp.mFileID);
        }
        else{
            callback(false, UString());
        }
    });
}

void GdriveImpl::DeleteItem(const UString &fileID, FileOperationCallbackProc_t callback){
    mSessionManager->GetAccessToken([this, fileID, callback](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback, fileID](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               if(code == kHTTPResponseOK || code == kHTTPResponseBlank)
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
        params[UString(L"access_token")] =  accessToken;
        
        webServiceEngine->SendRequestByMethod("DELETE", UString(GDrive::kFileOpsURL) + fileID, BinaryData_t(), params, WebServicesInterface::UserDataType_t());

    });
}

void GdriveImpl::CreateMetadataEntry(const UString& fileID, const UString& parentID, const UString& mimeType, const UString &name, ServiceResponseCallback_t callback)
{
    mSessionManager->GetAccessToken([this, parentID, mimeType, name, callback, fileID](const UString& accessToken){
        auto webServiceEngine = WebServicesInterface::CreateEngineInstance([this, callback](int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers)
                                                                           {
                                                                               callback(code, response);
                                                                           });
        
        WebServicesInterface::ParametersType_t params;
        params[UString(L"access_token")] =  accessToken;
        
        Json::StyledWriter jsonWriter;
        
        Json::Value fileResource;
        
        if(!parentID.empty())
        {
            Json::Value parents;
            Json::Value parentObject;
            
            if(parentID == UString("/"))
                parentObject["id"] = "root";
            else
                parentObject["id"] = parentID.getUTF8String();
            parents[0] = parentObject;
            
            fileResource["parents"] = parents;
        }
        
        
        if(!name.empty())
            fileResource["title"] = name.getUTF8String();
        
        if(!mimeType.empty())
            fileResource["mimeType"] = mimeType.getUTF8String();
        
        auto reqBody = jsonWriter.write(fileResource);
        
        webServiceEngine->SetRequestHeader("Content-Type", UString("application/json"));
        
        auto strURL = UString(GDrive::kFileOpsURL);
        if(!fileID.empty())
        {
            strURL += fileID;
            webServiceEngine->SendRequestByMethod("PATCH", strURL, reqBody, params, WebServicesInterface::UserDataType_t());
        }
        else
        {
            webServiceEngine->SendPostRequest(strURL, reqBody, params, WebServicesInterface::UserDataType_t());
        }
    });

}

void GdriveImpl::CreateFolder(const UString& destFileId, const UString &folderName, FileOperationCallbackProc_t callback){
    CreateMetadataEntry(UString(), destFileId, L"application/vnd.google-apps.folder", folderName, [this, callback, folderName](int code, const UString& accessToken){
        if(code == kHTTPResponseOK)
        {
            
            callback(true);
        }
        else
        {
            Utils::Log(UString("Create folder operation for ") + folderName + UString(" failed!. Error: ") + UString(code));
            callback(false);
        }
    });
                        
}
