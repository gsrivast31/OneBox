//
//  SkyDriveImpl.h
//  AccountServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef __AccountServices__SkyDriveImpl__
#define __AccountServices__SkyDriveImpl__

#include "AccountServiceInterface.h"
#include "AccountServiceController.h"
#include "WebServicesInterface.h"
#include "OAuthDialog.h"
#include "json.h"

namespace NewGroup
{
    namespace Core
    {
        class SkyDriveImpl: public AccountServiceInterface
        {
            
            SkyDriveImpl();
            ~SkyDriveImpl();
            
            void GetDefaultReqParameters(WebServicesInterface::ParametersType_t &params);
            
            //Requests
            virtual void Authenticate(PlatformView parentView, AuthCallBackProc_t callBack);
            void AuthenticateWithToken(const UString& authToken, AuthCallBackProc_t callBack);
            
            void AuthenticateStage2(PlatformView parentView, AuthCallBackProc_t callBack, const UString &oauthToken, const UString &oauthSecret);
            
            virtual void RequestAccountInfo(AccInfoCallBackProc_t callBack);
            void ListFolderContent(const UString &folderID, const UString &folderPath, FileUpdateCallBackProc_t callBack);
            virtual void StartSendingUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint); //frequency in seconds
            void DownloadThumbnail(const UString &fileID, const UString &path, int width, int height, ThumbnailCallBackProc_t callback);
            void DownloadFile(const UString &fileID, const UString &path, FileCallBackProc_t callback);
            void GeneratePublicURL(const UString &fileID, FileURLCallBackProc_t callback);
            void CopyItem(const UString &srcfileID, const UString &, const UString &destFileLocation, CopyOperationCallbackProc_t callback);
            void MoveItem(const UString &srcfileID, const UString &, const UString &destFileLocation, MoveOperationCallbackProc_t callback);
            void RenameItem(const UString &srcfileID, const UString &destFileLocation, const UString& newName, MoveOperationCallbackProc_t callback);

            void ParseMetadata(Json::Value& metadata, FileProperties& fileProps, const UString&folderPath);
            void DeleteItem(const UString &fileID, FileOperationCallbackProc_t callback);
            void CreateFolder(const UString& destFileId, const UString &folderName, FileOperationCallbackProc_t callback);
            //Response Handlers
            void HandleAuthStage1Response(PlatformView parentView, AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers);
            void HandleAuthStage2Response(AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers);
            void UploadFile(const UString &localFilePath, const UString &remoteFilePath, FileUploadCallBackProc_t callback);
            //Utility methods
            UString GetServiceRootFolder() const;
            UString GetThumbnailRootFolder() const;
            UString GetDownloadRootFolder() const;
            
            UString GetIconName(const UTF8String& SkyDriveIcon);
            void ParseAccountInfo(const UString& response, AccountInfo& accInfo);
            //Data
            OAuthDialog::OAuthDialogSharedPtr mOauthDlgPtr;
            friend AccountServiceInterface::ASEngineSharedPtr AccountServiceController::CreateServiceInstance(NewGroup::Core::ServiceProvider provider);
            
            UString mAccessToken;
            UString mTokenSecret;
            UString mUID;
            UString mTokenType;
            UString mRefreshToken;
            
            static std::map<UTF8String, UTF8String> mSkyDriveToIconsMapping;
            UString mRootFolderID;
        };

    }
}
#endif /* defined(__AccountServices__SkyDriveImpl__) */
