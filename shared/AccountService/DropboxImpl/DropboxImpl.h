//
//  DropboxImpl.h
//  AccountServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef __AccountServices__DropboxImpl__
#define __AccountServices__DropboxImpl__

#include "AccountServiceInterface.h"
#include "AccountServiceController.h"
#include "WebServicesInterface.h"
#include "OAuthDialog.h"
#include <json.h>
namespace NewGroup
{
    namespace Core
    {
        class DropboxImpl: public AccountServiceInterface
        {
            
            DropboxImpl();
            ~DropboxImpl();
            
            void GetDefaultReqParameters(WebServicesInterface::ParametersType_t &params);
            
            //Requests
            virtual void Authenticate(PlatformView parentView, AuthCallBackProc_t callBack);
            void AuthenticateWithToken(const UString& authToken, AuthCallBackProc_t callBack);
            
            void AuthenticateStage2(PlatformView parentView, AuthCallBackProc_t callBack, const UString &oauthToken, const UString &oauthSecret);
            
            virtual void RequestAccountInfo(AccInfoCallBackProc_t callBack);
            virtual void StartSendingUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint); //frequency in seconds
            void DownloadThumbnail(const UString &fileID, const UString &path, int width, int height, ThumbnailCallBackProc_t callback);
            void DownloadFile(const UString &fileID, const UString &path, FileCallBackProc_t callback);
            void GeneratePublicURL(const UString &fileID, FileURLCallBackProc_t callback);
            void CopyItem(const UString &srcfileID, const UString &srcfilePath, const UString &destFileLocation, CopyOperationCallbackProc_t callback);
            void MoveItem(const UString &srcfileID, const UString &srcfilePath, const UString &destFileLocation, MoveOperationCallbackProc_t callback);
            void MoveItemToLocation(const UString &srcfileID, const UString &destFileLocation, MoveOperationCallbackProc_t callback);
            void RenameItem(const UString &srcfileID, const UString &parentID, const UString& newName, MoveOperationCallbackProc_t callback);

            void DeleteItem(const UString &fileID, FileOperationCallbackProc_t callback);
            void CreateFolder(const UString& destFileId, const UString &folderName, FileOperationCallbackProc_t callback);
            //Response Handlers
            void HandleAuthStage1Response(PlatformView parentView, AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers);
            void HandleAuthStage2Response(AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers);
            void UploadFile(const UString &localFilePath, const UString &remoteFilePath, FileUploadCallBackProc_t callback);
            void ParseMetaData(Json::Value &fileMetadata, FileProperties& fileProp, const UString& fileID);
            //Utility methods
            UString GetServiceRootFolder() const;
            UString GetThumbnailRootFolder() const;
            UString GetDownloadRootFolder() const;
            
            UString GetIconName(const UTF8String& dropboxIcon);
            void ParseAccountInfo(const UString& response, AccountInfo& accInfo);
            //Data
            OAuthDialog::OAuthDialogSharedPtr mOauthDlgPtr;
            friend AccountServiceInterface::ASEngineSharedPtr AccountServiceController::CreateServiceInstance(NewGroup::Core::ServiceProvider provider);
            UString mAccessToken;
            UString mTokenSecret;
            UString mUID;
            
            static std::map<UTF8String, UTF8String> mDropBoxToIconsMapping;
        };

    }
}
#endif /* defined(__AccountServices__DropboxImpl__) */
