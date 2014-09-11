//
//  AccountServiceInterface.h
//  TestWebServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef TestWebServices_AccountServiceInterface_h
#define TestWebServices_AccountServiceInterface_h

#include "UString.h"
#include "Exception.h"
#include "ODAssert.h"
#include "Types.h"
#include <map>
#include <functional>

namespace NewGroup
{
    namespace Core
    {
        enum AccountType
        {
            kAccountTypeOnlineSharing
        };
        
        enum ServiceProvider
        {
            kSPDropbox,
            kSPSkyDrive,
            kSPGDrive,
            kSPUnknown
        };
        
        struct AccountInfo
        {
            UString mAccountTitle;
            UString mAccountDescription;
            UString mAuthor;
            
            UString mAccountID;
            unsigned long long mTotalSpace;
            unsigned long long mFreeSpaceLeft;
        };
        
        struct FileProperties
        {
            UString mFilePath;
            UString mFileName;
            UString mMimeType;
            
            UString mThumbnail;
            unsigned long long mFileSize; //in bytes
            UString mFileID;
            UString mFileRev;
            bool mIsFolder;
            
            UString mParentID;
            
            DateTime mLastModifiedTime; //Last modified on server
            
            bool mIsRootElement;
        };
        
        struct ServiceInfo
        {
            UString mServiceName;
            ServiceProvider mServiceProvider;
            AccountType mServiceType;
            UString mIconPath;
        };
        
        class AccountServiceInterface:public std::enable_shared_from_this<AccountServiceInterface>
        {
        public:
            typedef std::shared_ptr<AccountServiceInterface> ASEngineSharedPtr;
            typedef std::function<void (AuthenticationResult, const UString &authToken, const AccountInfo &)> AuthCallBackProc_t;
            typedef std::function<void (const AccountInfo &)> AccInfoCallBackProc_t;
            typedef std::function<void (bool deleted, const UString& checkPoint, const FileProperties &)> FileUpdateCallBackProc_t;
            typedef std::function<void (const UString &fileID, const UString &localPath)> ThumbnailCallBackProc_t;
            typedef std::function<void (const UString &fileID, const UString &localPath, const UString& fileRev)> FileCallBackProc_t;
            typedef std::function<void (bool success, const FileProperties &)> FileUploadCallBackProc_t;
            typedef std::function<void (const UString &fileID, const UString &fileURL)> FileURLCallBackProc_t;
            
            typedef std::function<void (bool success)> FileOperationCallbackProc_t;
            typedef std::function<void (bool success, const FileProperties &)> CopyOperationCallbackProc_t;
            typedef std::function<void (bool success, const UString &)> MoveOperationCallbackProc_t;
            
            virtual void Authenticate(PlatformView parentView, AuthCallBackProc_t callBack) = 0;
            virtual void AuthenticateWithToken(const UString& authToken, AuthCallBackProc_t callBack) = 0;
            
            virtual void RequestAccountInfo(AccInfoCallBackProc_t callBack) = 0;
            
            //Pass checkpoint as empty string for the first time
            virtual void StartSendingUpdates(FileUpdateCallBackProc_t callBack, const UString& checkpoint) = 0;
            
            virtual void GeneratePublicURL(const UString &fileID, FileURLCallBackProc_t callback) = 0;
            
            virtual void DownloadThumbnail(const UString &fileID, const UString &path, int width, int height, ThumbnailCallBackProc_t callback) = 0;
            virtual void DownloadFile(const UString &fileID, const UString &path, FileCallBackProc_t callback) = 0;
            virtual void UploadFile(const UString &localFilePath, const UString &remoteFilePath, FileUploadCallBackProc_t callback) = 0;
            
            virtual void CopyItem(const UString &srcfileID, const UString &srcfilePath, const UString &destFileLocation, CopyOperationCallbackProc_t callback) = 0;
            virtual void MoveItem(const UString &srcfileID, const UString &srcfilePath, const UString &destFileLocation, MoveOperationCallbackProc_t callback) = 0;
            virtual void RenameItem(const UString &srcfileID, const UString &parentID,
                                    const UString& newName, MoveOperationCallbackProc_t callback) = 0;
            virtual void DeleteItem(const UString &fileID, FileOperationCallbackProc_t callback) = 0;
            virtual void CreateFolder(const UString& destFileId, const UString &folderName, FileOperationCallbackProc_t callback) = 0;
            virtual ~AccountServiceInterface(){}
        };
    }
}


#endif
