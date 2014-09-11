//
//  GdriveImpl.h
//  AccountServices
//
//  Created by Vikash Jindal on 04/28/13.
//  Copyright (c) 2013 Vikash Jindal. All rights reserved.
//

#ifndef __AccountServices__GdriveImpl__
#define __AccountServices__GdriveImpl__

#include "AccountServiceInterface.h"
#include "AccountServiceController.h"
#include "WebServicesInterface.h"
#include "OAuthDialog.h"
#include <json.h>

#include <set>
#include <atomic>
#include "SessionManager.h"

namespace NewGroup
{
    namespace Core
    {
        class GdriveImpl: public AccountServiceInterface
        {
            
            GdriveImpl();
            ~GdriveImpl();
            
            
            //Requests
            typedef std::function<void(int code, const UString& response)> ServiceResponseCallback_t;
            
            virtual void Authenticate(PlatformView parentView, AuthCallBackProc_t callBack);
            void AuthenticateWithToken(const UString& authToken, AuthCallBackProc_t callBack);
            void ParseToken(const UString& response, UString& accessToken);
            void GetUserInfo(AuthCallBackProc_t callBack);
            void GetAboutInfo(AuthCallBackProc_t callBack, UString email, UString id);
            void GetAboutInfo(AccInfoCallBackProc_t callBack, UString email, UString id);
            virtual void RequestAccountInfo(AccInfoCallBackProc_t callBack);
            virtual void StartSendingUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint); //frequency in seconds
            void RequestUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint, const UString& pageToken);
            void DownloadThumbnail(const UString &fileID, const UString &path, int width, int height, ThumbnailCallBackProc_t callback);
            void DownloadFile(const UString &fileID, const UString &path, FileCallBackProc_t callback);

            void GeneratePublicURL(const UString &fileID, FileURLCallBackProc_t callback);
            void CopyItem(const UString &srcfileID, const UString &srcfilePath, const UString &destFileLocation, CopyOperationCallbackProc_t callback);
            void MoveItem(const UString &srcfileID, const UString &srcfilePath, const UString &destFileLocation, MoveOperationCallbackProc_t callback);
            void RenameItem(const UString &srcfileID, const UString &destFileLocation, const UString& newName, MoveOperationCallbackProc_t callback);

            void DeleteItem(const UString &fileID, FileOperationCallbackProc_t callback);
            void CreateFolder(const UString& destFileId, const UString &folderName, FileOperationCallbackProc_t callback);
            //Response Handlers
            void HandleAuthStage1Response(AuthCallBackProc_t callBack, int code, const WebServicesInterface::UserDataType_t& userData, const UString& response, const WebServicesInterface::HeaderListType_t& headers);
            void UploadFile(const UString &localFilePath, const UString &remoteFilePath, FileUploadCallBackProc_t callback);
            //Utility methods
            UString GetServiceRootFolder() const;
            UString GetThumbnailRootFolder() const;
            UString GetDownloadRootFolder() const;
            UString GenerateParent(Json::Value &entries, UTF8String &id);

            UString GetIconName(const UTF8String& dropboxIcon);
            void ParseAccountInfo(const UString& response, AccountInfo& accInfo);
            void GetChildrenList(const UString& root, const UString& path, const UString& pageToken, const UString& checkpoint, FileUpdateCallBackProc_t callBack);
            
            bool ParseMetadata(Json::Value& entry, const UString& path, FileProperties& fileProp);
            void RequestMetadata(const UString& fileID, const UString& path, const UString& checkpoint, FileUpdateCallBackProc_t callBack);
            
            void RefreshToken(RefreshTokenCallbackProc_t proc);
            
            void CreateMetadataEntry(const UString& fileID, const UString& parentID, const UString& mimeType, const UString &name, ServiceResponseCallback_t callback);
            //Data
            OAuthDialog::OAuthDialogSharedPtr mOauthDlgPtr;
            friend AccountServiceInterface::ASEngineSharedPtr AccountServiceController::CreateServiceInstance(NewGroup::Core::ServiceProvider provider);
            UString mRefreshToken;
            int mExpiresIn;
            UString mTokenType;
            UString mIdToken;
            UTF8String mRootFolderId;
            UString mTokenSecret;
            UString mUID;
            static std::map<UTF8String, UString> mFolderIdtoTitleMapping;

            static std::set<UTF8String> mAvailableIcons;
            static std::map<UTF8String, UTF8String> mGDriveToIconsMapping;
            std::atomic_ulong mPendingChildren;
            
            std::unique_ptr<SessionManager>  mSessionManager;
        };

    }
}
#endif /* defined(__AccountServices__GdriveImpl__) */
