//
//  AppController.h
//  OneDrive
//
//  Created by Ajay on 12/17/12.
//  Copyright (c) 2012 Labs. All rights reserved.
//
#pragma once

#ifndef App_Controller_h
#define App_Controller_h

#include <functional>
#include "ODDataController.h"
#include "AccountServiceController.h"
#include "UString.h"
#include "ShareServiceInterface.h"
#include <mutex>
#include <thread>
#include "NotificationManagerImpl.h"

#include <deque>

class AccountServiceController;

const int kRecentMaxSize = 5;

namespace NewGroup
{
    using namespace NewGroup::Core;
    
    struct Size
    {
        int width;
        int height;
        Size():width(0), height(0){}
        Size(int w, int h):width(w), height(h){}
    };
    
    struct DownloadInfo
    {
        enum
        {
            kDownloadCompleted,
            kDownloadRunning,
            kDownloadFailed,
            kDownloadCancelled
        };
        
        int downloadStatus;
        ID mediaID;
        UString localPath;
    };
    
    class AppController
    {
        
        typedef std::function<void (RESULT, ODAccount)> callback_addAccount;
        typedef callback_addAccount callback_getAccount;
        
        typedef std::function<void (RESULT, ODMedia, bool deleted)> media_get_callback;
        
        typedef std::function<void (RESULT, ODThumb)> thumbnail_get_callback;
    
        typedef std::function<void (AuthenticationResult, const UString &authToken, const ODAccount &accountInfo, callback_addAccount callback )> AuthCallBackProc_t;
        
        typedef std::function<void (UString localPath, int64_t finished, bool success)> file_upload_callback;
        
        typedef std::function<void (bool success, const UString &fileID, const UString &localPath)> file_download_callback;
        
        typedef std::shared_ptr<AccountServiceController> NSController;
        
        typedef ShareServiceInterface::ShareCallBackProc_t ShareCallBackProc_t;
        typedef std::function<void (bool success, ODMedia media)> FileOperationCallback;
        
        typedef std::deque<ID> IDQueue;
        
        std::recursive_mutex mLock;
        typedef std::lock_guard<std::recursive_mutex> LockGuard;

        
    public:
        static AppController& instance();
        
        typedef std::shared_ptr<DownloadInfo> DownloadInfoData_t;
        typedef NewGroup::Core::NotificationManager<DownloadInfoData_t> FileDownloadManager_t;
        
        void AddAccountForService(ServiceProvider serviceID, const UString &name, callback_addAccount callBack );
        
        void GetAllServices(ODServiceList&);
        
        void GetAllAccounts(ODAccountList&);
        
        RESULT GetAllAccountForService(ServiceProvider serviceID, callback_getAccount callback);
        
        void   SetParentView(PlatformView view);
        
        long GetLastSyncedTimeOfAccount(const int& id);
        
        void SyncAccount(const int& id, media_get_callback);
        
        RESULT GetMediaForAccount(const int& id, media_get_callback);
        RESULT UpdateMedia(ODMedia media);
        RESULT RemoveMedia(ODMedia media);
        
        RESULT cancelLastQuery();
        
        RESULT GetMediaWithProperty(const UString &relativePath,const UString& property, media_get_callback, int accountId = -1); // Property can be anything which we support querying for
        RESULT GetMediaWithMimeType(const std::string& mimeType, media_get_callback, int accountId = -1); // Property can be anything which we support querying for
        RESULT GetFavoriteMedia(media_get_callback, int accountId = -1); 
        
        RESULT GetChildMedia(const ID& parentId, const int& accountId, ODMediaList& childList);
        
        void SetThumbnailSize(Size size);
        
        void DownloadThumbnailForMedia(ODMedia& media, int width, int height, media_get_callback);
        
        RESULT GetThumbnailForMedia(const ID& mediaId, int width, int height, thumbnail_get_callback);
        
        void UploadFile(const ID& media_id, const int& accountId, const UString& localPath, const UString& remotePath, file_upload_callback);
        
        void DownloadFile(const int& accountId, const ID& mediaId);
        
        void MoveFile(const int& accountId, const ID& mediaId, const UString& destPath, FileOperationCallback);
        void CopyFile(const int& accountId, const ID& mediaId, const UString& destPath, FileOperationCallback);
        void DeleteFile(const int& accountId, const ID& mediaId, FileOperationCallback);
        void CreateFolder(const int& accountId, const UString& destFileId, const UString &folderName, FileOperationCallback);
        void RenameFileOrFolder(const int& accountId, const ID& mediaId, const UString& newName, FileOperationCallback);// I don't think we need Account ID to pass. MediaId needs to be distinct.
        RESULT SetMediaAsFavorite(const ID& id);
        RESULT RemoveMediaFromFavorite(const ID& id);
        bool IsMediaFavorite(const ID& id);
        
        void GetRecents(ODMediaList&);
        void AddToRecent(const ID&);
        
        void ShareFileWithComments(int services, const ID& mediaId, int accountID, const UString& text, ShareCallBackProc_t callback);
        FileDownloadManager_t *GetDownloadManager() const
        {
            return mDownloadNotificationManager.get();
        }
    private: // member fucntions

        AppController();
        AppController(const AppController&);
        virtual ~AppController();
        const AppController& operator=(const AppController&);
        
        void NSAccountCreated(AuthenticationResult res, const UString &authToken, ODAccount& accountInfo, callback_addAccount callback, AccountServiceInterface::ASEngineSharedPtr);
        
        void DBAccountInfoReceived(ODAccount, callback_getAccount);
        
        void MediaThumbnailReceived(const UString &fileId, const UString &localPath, ODMedia& media,
                                    int width, int height, media_get_callback callback);

        // Singleton management functions
        static void createInstance();
        static void killPhoenixSingleton();
        static void onDeadReference();

        std::unique_ptr<NotificationManagerImpl<DownloadInfoData_t> > mDownloadNotificationManager;
        void DisptachDownloadNotification(int status, const ID& mediaID = -1, const UString& filePath = "");
        
        //methods
    private:
        ODDataController *          mDataController; // singleton object
        NSController                mNSController;
        static bool                 mbDestroyed;
        static AppController *      mpInstance;
        PlatformView                mpView;
    
        std::map<int, AccountServiceInterface::ASEngineSharedPtr> accountServiceMap;
        std::map<int, UString> accountCheckPointMap;
        
        IDQueue mRecentIdQueue;
        
        Size mThumbnailSize;
    };
    
    
}



#endif
