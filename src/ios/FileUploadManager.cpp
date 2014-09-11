//
//  FileUploadManager.cpp
//  OneDrive
//
//  Created by Ajay on 6/2/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#include "FileUploadManager.h"

static std::mutex sODFileUploadPMMutex;
FileUploadProgressManager* FileUploadProgressManager::mpInstance = NULL;
bool FileUploadProgressManager::mbDestroyed = false;

FileUploadProgressManager& FileUploadProgressManager::instance()
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

FileUploadProgressManager::FileUploadProgressManager()
{
    if(!mpInstance)
    {
        mUploadNotificationManager.reset(new NotificationManagerFileUploadImpl<UploadInfoData_t>());
    }
    else
    {
        // Assert error
    }
}

FileUploadProgressManager::~FileUploadProgressManager()
{
    mpInstance = 0;
    mbDestroyed = true;
}

void FileUploadProgressManager::createInstance()
{
    sODFileUploadPMMutex.lock();
    
    if (!mpInstance)
    {
        static FileUploadProgressManager instance;
        mpInstance = &instance;
    }
    
    sODFileUploadPMMutex.unlock();
}

void FileUploadProgressManager::killPhoenixSingleton()
{
    mpInstance->~FileUploadProgressManager();
}

void FileUploadProgressManager::onDeadReference()
{
    // setup the instance shell
    createInstance();
    
    //mpInstance now points to raw memory used before, so create another one in that same memory
    new(mpInstance) FileUploadProgressManager;
    
    // Queue this object for delete again
    atexit(killPhoenixSingleton);
    
    mbDestroyed = false;
}

int FileUploadProgressManager::TotalMediaUploading()
{
    return mUploadFileNodes.size();
}

bool FileUploadProgressManager::IsUploading(UString& filePath)
{
    return (mUploadFileNodes.find(filePath) != mUploadFileNodes.end());
}

void FileUploadProgressManager::AddObserver(WeakObserverType_t obr)
{
    mUploadNotificationManager.get()->AddObserver(obr);
}

void FileUploadProgressManager::AddMediaForUploading(UString &filePath)
{
    if(mUploadFileNodes.find(filePath) == mUploadFileNodes.end())
    {
        // Insert the mediaID
        auto sp = std::make_shared<UploadInfo>(UploadInfo::kUploadRunning,filePath,0, 1);
        mUploadFileNodes.insert(std::pair<UString,UploadInfoData_t>(filePath,sp ));
    }
     DisptachUploadNotification(filePath, UploadInfo::kUploadRunning,0);
}

void FileUploadProgressManager::updateStatus(UString filePath, double sizeUploaded, int status)
{
    if(mUploadFileNodes.find(filePath) == mUploadFileNodes.end())
    {
        // Insert the mediaID
        auto sp = std::make_shared<UploadInfo>(UploadInfo::kUploadRunning,filePath,sizeUploaded, 1);
        mUploadFileNodes.insert(std::pair<UString,UploadInfoData_t>(filePath,sp ));
    }
    if(sizeUploaded == 1.0 && status == UploadInfo::kUploadCompleted) // Its complete, then remove it from the map.
    {
        mUploadFileNodes.erase(filePath);
    }
    DisptachUploadNotification(filePath, status,sizeUploaded);
    
}


void FileUploadProgressManager::DisptachUploadNotification(UString mediaID , int status, int64_t sizeUploaded )
{
    mUploadNotificationManager.get()->Dispatch(std::make_shared<UploadInfo>(UploadInfo(status,mediaID,sizeUploaded,1)));
}