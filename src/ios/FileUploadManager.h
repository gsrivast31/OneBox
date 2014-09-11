//
//  FileUploadManager.h
//  OneDrive
//
//  Created by Ajay on 6/2/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#ifndef __OneDrive__FileUploadManager__
#define __OneDrive__FileUploadManager__

#include <iostream>
#include <map>
#include "Types.h"
#include "NotificationManager.h"
#include "NotificationManagerImpl.h"

using namespace NewGroup::Core;


namespace NewGroup
{
    namespace Core
    {
        template <typename NotificationDataT>
        class NotificationManagerFileUploadImpl:public NotificationManager<NotificationDataT>
        {
        public:
            // TODO: Need to overwrite this stuff.
            // Dispatch this notification to only those observers which match certain condition
            void Dispatch(NotificationDataT notificationData)
            {
                NotificationManager<NotificationDataT>::Dispatch(notificationData);
            }
        };
        
    }
}



struct UploadInfo
{
    enum
    {
        kUploadCompleted,
        kUploadRunning,
        kUploadFailed,
        kUploadCancelled
    };
    
    int uploadStatus; // Current Status
    UString filePath; // Media for which we are tracking the status
    int64_t totalSize; // May not be used for now
    int64_t sizeUploaded; //size already uploaded
    UploadInfo(int status, UString mID, int64_t szUploaded, int64_t tSize = 1)
    {
        uploadStatus = status;
        filePath = mID;
        totalSize = tSize;
        sizeUploaded = szUploaded;
    }
};



class FileUploadProgressManager
{
public:
    
    typedef std::shared_ptr<UploadInfo> UploadInfoData_t;
    typedef std::map<UString, UploadInfoData_t> UploadFileNodes;
    typedef NewGroup::Core::NotificationManager<UploadInfoData_t> UploadFileProgressManager_t;
    typedef std::function<void (UploadInfoData_t)> ObserverType_t;
    typedef std::weak_ptr<ObserverType_t> WeakObserverType_t;
    
    static FileUploadProgressManager& instance();
    
    void updateStatus(UString filePath, double sizeUploaded, int status);
    
    void AddObserver(WeakObserverType_t);
    
    bool IsUploading(UString &filePath);
    
    void AddMediaForUploading(UString &filePath);
    
    int TotalMediaUploading();
    UploadFileProgressManager_t *GetUploadNotificationManager()
    {
        return mUploadNotificationManager.get();
    }
    
    UploadFileNodes GetUploadMediaNodes()
    {
        return mUploadFileNodes;
    }
    
private:
    FileUploadProgressManager();
    ~FileUploadProgressManager();
    // Singleton management functions
    static void createInstance();
    static void killPhoenixSingleton();
    static void onDeadReference();
    
    //This notifies the observer if it has same mediaID.
    void DisptachUploadNotification(UString mediaID , int status, int64_t sizeUploaded = 0);

    
private:
    static bool                 mbDestroyed;
    static FileUploadProgressManager *      mpInstance;
    UploadFileNodes mUploadFileNodes;
    std::unique_ptr<NewGroup::Core::NotificationManagerFileUploadImpl<UploadInfoData_t> > mUploadNotificationManager;

};


#endif /* defined(__OneDrive__FileUploadManager__) */
