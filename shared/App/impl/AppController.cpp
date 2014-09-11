//
//  AppController.cpp
//  NewGroup
//
//  Created by Ajay on 12/17/12.
//  Copyright (c) 2012 Labs. All rights reserved.
//

#include "AppController.h"
#include "ODDataController.h"
#include "AccountServiceController.h"
#include "ODLogger.h"

#include <mutex>
#include <cstdlib>

using namespace std::placeholders;

static std::mutex sODAppMutex;

namespace NewGroup
{

    AppController* AppController::mpInstance = NULL;
    bool AppController::mbDestroyed = false;

    AppController& AppController::instance()
    {
        if (!mpInstance)
        {
            if (mbDestroyed)
                onDeadReference();
            else
                createInstance();
        }
        return *mpInstance;
    }

    AppController::AppController()
    {
        if(!mpInstance)
        {
            mDataController = ODDataController::instance();
                
#if Macintosh
            NSString *docsDir;
            NSArray *dirPaths;
            dirPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            docsDir = [dirPaths objectAtIndex:0];
#else
            // Unimplemented
#endif
            mDataController->SetDatabasePath(UString([docsDir UTF8String])); // give name
            mNSController = NewGroup::Core::AccountServiceController::GetInstance();
            
            mThumbnailSize.width = 256;
            mThumbnailSize.height = 256;
            
            mDownloadNotificationManager.reset(new NotificationManagerImpl<DownloadInfoData_t>());
        }
        else
        {
                // Assert error
        }
    }

    AppController::~AppController()
    {
        mpInstance = 0;
        mbDestroyed = true;
    }
    
    void AppController::createInstance()
    {
        sODAppMutex.lock();
    
        if (!mpInstance)
        {
            static AppController instance;
            mpInstance = &instance;
        }
    
        sODAppMutex.unlock();
    }

    void AppController::killPhoenixSingleton()
    {
        mpInstance->~AppController();
    }

    void AppController::onDeadReference()
    {
        // setup the instance shell
        createInstance();
    
        //mpInstance now points to raw memory used before, so create another one in that same memory
        new(mpInstance) AppController;
    
        // Queue this object for delete again
        atexit(killPhoenixSingleton);
    
        mbDestroyed = false;
    }
    
    void AppController::SetParentView(PlatformView view)
    {
        mpView = view;
    }
    
    void AppController::SetThumbnailSize(Size size)
    {
        mThumbnailSize = size;
    }
    
    void AppController::NSAccountCreated(AuthenticationResult res, const UString &authToken, ODAccount& accountInfo, callback_addAccount callback, AccountServiceInterface::ASEngineSharedPtr engine)
    {
        if(res == kAuthenticationPassed)
        {
            if (mDataController->AccountAlreadyExists(accountInfo.account_remote_id) == false)
            {
                if(mDataController->AddAccountForService(accountInfo))
                {
                    accountServiceMap[accountInfo.account_id] = engine;
                    callback(RESULT_NoErr,accountInfo);
                }
                else
                    callback(RESULT_AccountCreationFailed,accountInfo);
            }
            else
                callback(RESULT_AccountExists, accountInfo);
        }
        else
             callback(RESULT_AccountCreationFailed,accountInfo);
    }
    
    void AppController::AddAccountForService(ServiceProvider serviceID, const UString &name, callback_addAccount callBack )
    {
        int cnt;
        AccountServiceInterface::ASEngineSharedPtr servicePtr = mNSController->CreateServiceInstance(serviceID);
        cnt = servicePtr.use_count();
        if(servicePtr.get())
        {
            //servicePtr->Authenticate(mpView, std::bind(&AppController::NSAccountCreated,this,_1,_2, accountInfo,callBack, servicePtr));
            servicePtr->Authenticate(mpView, [this, callBack, servicePtr, serviceID](AuthenticationResult res, const UString &authToken, const AccountInfo &accInfo)
                        {
                            if(res == kAuthenticationPassed)
                            {
                                ODAccount accountInfo;
                                accountInfo.account_service_id = serviceID;
                                accountInfo.account_status = true;
                                accountInfo.account_name = accInfo.mAccountDescription;
                                accountInfo.account_remote_id = accInfo.mAccountID;
                                accountInfo.account_token = authToken;
                                accountInfo.checkpoint = "";
                                
                                NSAccountCreated(res, authToken, accountInfo, callBack, servicePtr);
                            };
                        });
            cnt = servicePtr.use_count();
        }
    }
    
    void AppController::DBAccountInfoReceived(ODAccount account, callback_getAccount callback)
    {
        int id = account.account_id;
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[id];
        
        if (servicePtr.get() == NULL) {
            int serviceID = account.account_service_id;
            
            servicePtr = mNSController->CreateServiceInstance((ServiceProvider)serviceID);
            
            servicePtr->AuthenticateWithToken(account.account_token, [this, callback, servicePtr, serviceID, account](AuthenticationResult res, const UString &authToken, const AccountInfo &accInfo)
                                              {
                                                  if (res == kAuthenticationPassed) {
                                                      accountServiceMap[account.account_id] = servicePtr;
                                                      callback(RESULT_NoErr, account);
                                                  }
                                                  else
                                                  {
                                                      callback(RESULT_AccountAuthenticationFailed, account);
                                                  }
                                              });
            
        }
    }
    
    RESULT AppController::GetAllAccountForService(ServiceProvider serviceID, callback_getAccount callback)
    {
        mDataController->GetAccountsForService(serviceID, [this, callback](RESULT res, ODAccount account)
                                                        {
                                                            if (res == RESULT_NoErr) {
                                                                DBAccountInfoReceived(account, callback);
                                                            }
                                                            else {
                                                                callback(RESULT_AccountRetreivalFailed, account);
                                                            }
                                                        });
        return RESULT_NoErr;
        
    }
    
    void AppController::GetAllAccounts(ODAccountList& accountList)
    {
        mDataController->GetAllAccounts(accountList);
    }
    
    long AppController::GetLastSyncedTimeOfAccount(const int& id)
    {
        return 0;
    }
    
    void AppController::SyncAccount(const int& id, media_get_callback callback)
    {
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[id];
      
        if (servicePtr.get()) {
                UString checkPt = mDataController->GetAccountCheckPoint(id);
                Utils::Log(UString("Account checkpoint = ") + checkPt);
                
                servicePtr->StartSendingUpdates([this, callback, id](bool deleted, const UString& checkPoint, const FileProperties &fileProps)
                                                {
                                                    ODMedia media;
                                                    media.account_id = id;
                                                    media.full_path = fileProps.mFilePath;
                                                    media.file_id = fileProps.mFileID;
                                                    media.parent_file_id = fileProps.mParentID;
                                                    media.size = fileProps.mFileSize;
                                                    media.icon = fileProps.mThumbnail;
                                                    media.mime_type = fileProps.mMimeType;
                                                    media.is_folder = fileProps.mIsFolder;
                                                    media.revisionId = fileProps.mFileRev;
                                                    media.mod_date = fileProps.mLastModifiedTime;
                                                    media.file_name = fileProps.mFileName;
                                                    media.parent_id = -1;
                                                    
                                                    if (deleted) {
                                                        if (mDataController->RemoveMedia(media))
                                                            callback(RESULT_NoErr, media, deleted);
                                                        else
                                                            callback(RESULT_MediaDeletionFailed, media, deleted);
                                                    }
                                                    else {
                                                        if(media.file_id.empty())
                                                        {
                                                            //callback(RESULT_NoMediaChanges, media, deleted);
                                                        }
                                                        else if (mDataController->UpdateOrAddMediaForAccount(media))
                                                        {
                                                            if (media.parent_file_id == UString("/") || media.parent_id > 0) {
                                                                callback(RESULT_NoErr, media, deleted);
                                                                if (media.icon.empty()) {
                                                                    DownloadThumbnailForMedia(media,
                                                                                              mThumbnailSize.width,
                                                                                              mThumbnailSize.height,
                                                                                              callback);
                                                                }
                                                            }
                                                            //do nothing here.
                                                        }
                                                        else if(mDataController->UpdateOrAddTempMediaForAccount(media))
                                                        {
                                                            // do nothing here.
                                                        }
                                                        else
                                                            callback(RESULT_MediaCreationFailed, media, deleted);
                                                    }
                                                    
                                                    if(!checkPoint.empty())
                                                    {
                                                        mDataController->GetNewlyUpdatedMedia([this, callback](RESULT res, ODMedia media, bool deleted)
                                                                                              {
                                                                                                  callback(res, media, deleted);
                                                                                                  if (media.icon.empty()) {
                                                                                                      DownloadThumbnailForMedia(media,
                                                                                                                                mThumbnailSize.width,
                                                                                                                                mThumbnailSize.height,
                                                                                                                                callback);
                                                                                                  }
                                                                                              });
                                                        mDataController->SetAccountCheckPoint(id, checkPoint);
                                                    }
                                                }
                                                , checkPt);
        }
    }
    
    void AppController::MediaThumbnailReceived(const UString &fileId, const UString &localPath, ODMedia& media,
                                int width, int height, media_get_callback callback)
    {
        if (!localPath.empty()) {
            ODThumb thumb;
            thumb.media_id = media.media_id;
            thumb.localThumbPath = localPath;
            thumb.width = width;
            thumb.height = height;
            
            if (mDataController->UpdateOrAddThumbnailForMedia(thumb))
            {
                media.icon = thumb.localThumbPath;
                UpdateMedia(media);
                callback(RESULT_NoErr, media, false);
            }
            else
                callback(RESULT_ThumbnailCreationFailed, media, false);
        }
    }
    
    void AppController::DownloadThumbnailForMedia(ODMedia& media, int width, int height, media_get_callback callback)
    {
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[media.account_id];
        
        if (servicePtr.get()) {
            servicePtr->DownloadThumbnail(media.file_id, media.full_path, width, height,
                                          std::bind(&AppController::MediaThumbnailReceived,
                                                    this,_1,_2,
                                                    media, width, height, callback));
          }
    }
    
    RESULT AppController::GetMediaForAccount(const int& id, media_get_callback callback)
    {
        mDataController->GetMediaForAccount(id, [this, callback](RESULT res, ODMedia media, bool deleted)
                                                    {
                                                        callback(res, media, deleted);
                                                        if (res == RESULT_NoErr) {
                                                            if (media.icon.empty())
                                                                DownloadThumbnailForMedia(media,
                                                                                          mThumbnailSize.width,
                                                                                          mThumbnailSize.height, callback);
                                                        }
                                                    }
                                            );
    
        return RESULT_NoErr;
    }
    
    RESULT AppController::UpdateMedia(ODMedia media)
    {
        if (mDataController->UpdateMedia(media))
            return RESULT_NoErr;
        return RESULT_DBUpdateFailed;
    }
    
    RESULT AppController::RemoveMedia(ODMedia media)
    {
        if (mDataController->RemoveMedia(media))
            return RESULT_NoErr;
        return RESULT_DBUpdateFailed;
        
    }
    
    RESULT AppController::GetMediaWithProperty(const NewGroup::Core::UString &relativePath, const NewGroup::Core::UString &property, media_get_callback callback, int accountId)
    {
        mDataController->GetMediaByProperty(relativePath.getUTF8String(), property.getUTF8String(),callback, accountId);
        return RESULT_NoErr;
    }
    
    RESULT AppController::GetMediaWithMimeType(const std::string &mimeType, media_get_callback callback, int accountId)
    {
        mDataController->GetMediaByMimeType (mimeType,callback, accountId);
        return RESULT_NoErr;
    }
    
    RESULT AppController::GetChildMedia(const ID& parentId, const int& accountId, ODMediaList& childList)
    {
        mDataController->GetMediaListForParent(parentId, accountId, childList);
        return RESULT_NoErr;
    }
    
    RESULT AppController::GetFavoriteMedia(media_get_callback callback, int accountId)
    {
        mDataController->GetFavoriteMedia(callback, accountId);
        return RESULT_NoErr;
    }

    RESULT AppController::SetMediaAsFavorite(const ID& media_id)
    {
        // Return proper error codes. I think we would need to fix databases for this.
        mDataController->SetMediaAsFavorite(media_id);
        return RESULT_NoErr;
    }
    
    RESULT AppController::RemoveMediaFromFavorite(const ID& media_id)
    {
        // Return proper error codes. I think we would need to fix databases for this.
        mDataController->RemoveMediaFromFavorite(media_id);
        return RESULT_NoErr;
    }
    
    bool AppController::IsMediaFavorite(const ID& id)
    {
        return mDataController->IsMediaFavorite(id);
    }
    
    RESULT AppController::cancelLastQuery()
    {
        // I think we don't need this... But keeping it for the time being. It should be handled 
      //  mDataController->cancelLastQuery();
        return RESULT_NoErr;
    }

    RESULT AppController::GetThumbnailForMedia(const ID& mediaId, int width, int height, thumbnail_get_callback callback)
    {
        mDataController->GetThumbnailForMedia(mediaId, width, height, callback);
        return RESULT_NoErr;
    }
    
    void AppController::UploadFile(const ID& media_id, const int& accountId, const UString& localPath, const UString& remotePath,
                                   file_upload_callback callback)
    {
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountId];
        
        // Add to UploadFileProgressManager
        
        if (servicePtr.get()) {
            servicePtr->UploadFile(localPath, remotePath, [this, callback,localPath](bool success, const FileProperties &fileProp)
                                                            {
                                                                // Hardcoding finished scale for now, will need to update this later.
                                                                callback(localPath,1, success);
                                                            });
        }
    }
    
    void AppController::DisptachDownloadNotification(int status, const ID& mediaID, const UString& filePath)
    {
        DownloadInfoData_t downloadInfo = std::make_shared<DownloadInfo>();
        downloadInfo->downloadStatus = DownloadInfo::kDownloadCompleted;
        downloadInfo->mediaID = mediaID;
        downloadInfo->localPath = filePath;
        mDownloadNotificationManager->Dispatch(downloadInfo);
    }
    
    void AppController::DownloadFile(const int& accountId, const ID& mediaId)
    {
        //Check the status of download cache
        //Is download already running for this media?
        //If yes - return
        //else is file in cache?
        //if yes is its revision same?
        //yes- ok, good return from cahce
        //otherwise, download it from the cloud
        
        ODMedia media = mDataController->GetMediaForId(mediaId);
        
        {
            LockGuard lock(mLock);
            if(mDataController->GetMediaDownloadStatus(mediaId))
            {
                DisptachDownloadNotification(DownloadInfo::kDownloadRunning);
            }
            else
            {
                auto mediaPath = mDataController->GetMediaDownloadPath(mediaId);
                if(!mediaPath.empty())
                {
                    auto rev = mDataController->GetMediaDownloadRevision(mediaId);
                    if (rev == media.revisionId) {
                        DisptachDownloadNotification(DownloadInfo::kDownloadCompleted, mediaId, mediaPath);
                        return;
                    }
                }
                
            }
            
            //if(mDataController->GetMediaDownloadRevision(fileId) == )
        }
        
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountId];
        
        if (servicePtr.get()) {
            UString path = media.full_path;
            UString fileId = media.file_id;
            mDataController->SetMediaDownloadStatus(media.media_id, 1);
            servicePtr->DownloadFile(fileId, path, [this,  media](const UString &fileID, const UString &localPath,
                                                                    const UString& fileRev)
                                                                {
                                                                    {
                                                                        LockGuard lock(mLock);
                                                                        mDataController->SetMediaDownloadStatus(media.media_id, 0);
                                                                        mDataController->SetMediaDownloadDetails(media.media_id, localPath, fileRev);
                                                                    }
                                                                    DisptachDownloadNotification(DownloadInfo::kDownloadCompleted, media.media_id, localPath);
                                                                });
        }
        
    }
   
    void AppController::ShareFileWithComments(int services, const ID& mediaId, int accountID, const UString& text, ShareCallBackProc_t callback)
    {
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountID];
        const UString& fileId = mDataController->GetMediaForId(mediaId).file_id;
        servicePtr->GeneratePublicURL(fileId, [this, services, text, callback](const UString &fileID, const UString &fileURL){
            if(!fileURL.empty())
            {
                if(services & kShareViaFacebook)
                {
                    CreateShareServiceInstance(kShareViaFacebook)->ShareURL(fileURL, text, [this, callback](ShareServiceResult result, const UString &errorString){
                        callback(result, errorString);
                    });
                }
                else if(services & kShareViaTwitter)
                {
                    CreateShareServiceInstance(kShareViaTwitter)->ShareURL(fileURL, text, [this, callback](ShareServiceResult result, const UString &errorString){
                        callback(result, errorString);
                    });
                }
            }
        });
        
    }
}

    void AppController::MoveFile(const int& accountId, const ID& mediaId, const UString& destPath, FileOperationCallback callback){
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountId];
        
        if (servicePtr.get()) {
            ODMedia media = mDataController->GetMediaForId(mediaId);
            servicePtr->MoveItem(media.file_id, media.full_path, destPath, [this, media, callback](bool result, const UString& fileID)
                                     {
                                         callback(result, media);
                                     });
        }
    }
    
    void AppController::CopyFile(const int& accountId, const ID& mediaId, const UString& destPath, FileOperationCallback callback){
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountId];
        
        if (servicePtr.get()) {
            ODMedia media = mDataController->GetMediaForId(mediaId);
            servicePtr->CopyItem(media.file_id, media.full_path, destPath, [this, media, callback](bool result, const FileProperties& fileProps)
                                 {
                                     ODMedia updatedMedia = media;
                                     if (result) {
                                         updatedMedia.full_path = fileProps.mFilePath;
                                         updatedMedia.file_id = fileProps.mFileID;
                                         updatedMedia.parent_file_id = fileProps.mParentID;
                                         updatedMedia.size = fileProps.mFileSize;
                                         //updatedMedia.icon = fileProps.mThumbnail;
                                         //updatedMedia.mime_type = fileProps.mMimeType;
                                         //updatedMedia.is_folder = fileProps.mIsFolder;
                                         updatedMedia.revisionId = fileProps.mFileRev;
                                         updatedMedia.mod_date = fileProps.mLastModifiedTime;
                                         updatedMedia.parent_id = mDataController->GetMediaForFileId(media.account_id, media.parent_file_id).media_id;
                                         UpdateMedia(updatedMedia);
                                     }

                                     callback(result, updatedMedia);
                                 });
        }
    }
    
    void AppController::DeleteFile(const int& accountId, const ID& mediaId, FileOperationCallback callback){
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountId];
        
        if (servicePtr.get()) {
            ODMedia media = mDataController->GetMediaForId(mediaId);
            servicePtr->DeleteItem(media.file_id, [this, media, callback](bool result)
                                 {
                                     ODMedia deletedMedia = media;
                                     if (result) {
                                         mDataController->RemoveMedia(deletedMedia);
                                     }
                                     callback(result, media);
                                 });
        }
    }

    void AppController::RenameFileOrFolder(const int& accountId, const ID& mediaId, const UString& newName, FileOperationCallback callback){
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountId];
        if (servicePtr.get()) {
            ODMedia media = mDataController->GetMediaForId(mediaId);
            //ID parentId = media.parent_id;
            //UString destPath = mDataController->GetMediaForId(parentId).full_path + UString("/") + newName;
            servicePtr->RenameItem(media.file_id, media.parent_file_id, newName,  [this, media, newName, callback](bool result, const UString& newFileLocation)
                                 {
                                     ODMedia updatedMedia = media;
                                     
                                     if (result) {
                                         updatedMedia.file_name = newName;
                                         updatedMedia.full_path = newFileLocation;
                                         mDataController->UpdateOrAddMediaForAccount(updatedMedia);
                                     }
                                     
                                     callback(result, updatedMedia);
                                 });
        }
    }

    void AppController::CreateFolder(const int& accountId, const UString& destFileId, const UString &folderName, FileOperationCallback callback){
        AccountServiceInterface::ASEngineSharedPtr servicePtr = accountServiceMap[accountId];
        
        if (servicePtr.get()) {
            servicePtr->CreateFolder(destFileId, folderName, [this, callback](bool result)
                                   {
                                       ODMedia media;
                                       callback(result, media);
                                   });
        }
    }

    void AppController::GetRecents(ODMediaList& recentList){
        /*for (IDQueue::iterator iter = mRecentIdQueue.begin(); iter != mRecentIdQueue.end(); iter++) {
            ODMedia media = mDataController->GetMediaForId(*iter);
            recentList.push_back(media);
        }*/
        if (mDataController) {
            mDataController->GetRecentItems(recentList, kRecentMaxSize);
        }
    }

    void AppController::AddToRecent(const ID& id){
        /*if (mRecentIdQueue.size() >= kRecentMaxSize) {
            mRecentIdQueue.pop_front();
        }
        mRecentIdQueue.push_back(id);*/
        if (mDataController) {
            mDataController->AddToRecent(id);
        }
    }
