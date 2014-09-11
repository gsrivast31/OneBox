//
//  DataController.cpp
//  DataStorage
//
//  Created by OneDrive on 12/7/12.
//
//

#include "ODDataController.h"
#include <mutex>
#include "ODLogger.h"

static std::mutex sODMutex;

ODDataController* ODDataController::mpInstance = NULL;
bool ODDataController::mbDestroyed = false;

using namespace NewGroup;
using namespace NewGroup::Core;
using namespace std::placeholders;

namespace
{
    MediaType to_MediaType(const std::string mimeType)
	{
		if(mimeType == mime_type::image)
			return eModelMediaImage;
		else if(mimeType == mime_type::video)
			return eModelMediaVideo;
		else if(mimeType == mime_type::document)
			return eModelMediaDocument;
		else if(mimeType == mime_type::pdf)
			return eModelMediaPDF;
		else if(mimeType == mime_type::contact)
			return eModelMediaContact;
		else if(mimeType == mime_type::folder)
			return eModelMediaFolder;
		else
			return eModelMediaUnknown;
	}
    
	std::string to_MimeType(MediaType mediaType)
	{
		if(mediaType == eModelMediaImage)
			return mime_type::image;
		else if(mediaType == eModelMediaVideo)
			return mime_type::video;
		else if(mediaType == eModelMediaDocument)
			return mime_type::document;
		else if(mediaType == eModelMediaPDF)
			return mime_type::pdf;
		else if(mediaType == eModelMediaContact)
			return mime_type::contact;
		else if(mediaType == eModelMediaFolder)
			return mime_type::folder;
		else
			return mime_type::unknown;
	}
	
    ODAccount to_ODAccount(const account_description_t account_desc)
	{
		ODAccount account;
		account.account_id = account_desc.id;
		account.account_service_id = account_desc.service_id;
		account.account_name = account_desc.name;
		account.account_token = account_desc.token;
		account.account_status = account_desc.status;
        account.checkpoint = account_desc.checkpoint;
        account.account_remote_id = account_desc.remote_id;

        Utils::Log(UString("Account added with name = ") + account.account_name + UString(", id = ") + UString(account.account_id));
		return account;
	}
    
    account_description_t to_ODAccountDescription(const ODAccount &account)
	{
		account_description_t account_desc;
        account_desc.id = account.account_id;
        account_desc.service_id = account.account_service_id;
        account_desc.name = account.account_name.getUTF8String() ;
        account_desc.token = account.account_token.getUTF8String();
		account_desc.status = account.account_status ;
        account_desc.remote_id = account.account_remote_id.getUTF8String();
        account_desc.checkpoint = account.checkpoint.getUTF8String();
        
		return account_desc;
	}
    
	ODMedia to_ODMedia(const media_description_t media_desc)
	{				
		ODMedia media;
        media.media_id = media_desc.id;
        media.account_id = media_desc.account_id;
        media.file_id = media_desc.file_id;
        media.full_path = media_desc.full_path;
        media.mime_type = media_desc.mime_type;
        media.is_folder = media_desc.is_folder;
        media.size = media_desc.size;
        media.icon = media_desc.icon;
        media.revisionId = media_desc.revId;
        media.mod_date = media_desc.last_mod_date;
        media.parent_id = media_desc.parent_id;
        media.file_name = media_desc.file_name;
        media.parent_file_id = media_desc.parent_file_id;
		return media;
	}

	media_description_t to_ODMediaDescription(const ODMedia& media)
	{
        media_description_t media_desc;
        media_desc.id = media.media_id;
        media_desc.account_id = media.account_id;
        media_desc.size = media.size;
        media_desc.file_id = media.file_id.getUTF8String();
        media_desc.full_path = media.full_path.getUTF8String();
        media_desc.mime_type = media.mime_type.getUTF8String();
        media_desc.is_folder = media.is_folder;
        media_desc.icon = media.icon.getUTF8String();
        media_desc.revId = media.revisionId.getUTF8String();
        media_desc.last_mod_date = media.mod_date.GetUTCTime();
        media_desc.parent_id = media.parent_id;
        media_desc.file_name = media.file_name.getUTF8String();
        media_desc.parent_file_id = media.parent_file_id.getUTF8String();
        return media_desc;
	}
    
    ODThumb to_ODThumb(const thumb_description_t thumb_desc)
    {
        ODThumb thumb;
        thumb.thumb_id = thumb_desc.thumb_id;
        thumb.media_id = thumb_desc.media_id;
        thumb.width = thumb_desc.width;
        thumb.height = thumb_desc.height;
        thumb.localThumbPath = thumb_desc.thumbnailPath;
        return thumb;
    }
    
    thumb_description_t to_ODThumbDescription(const ODThumb& thumb)
    {
        thumb_description_t thumb_desc;
        thumb_desc.thumb_id = thumb.thumb_id;
        thumb_desc.media_id = thumb.media_id;
        thumb_desc.width = thumb.width;
        thumb_desc.height = thumb.height;
        thumb_desc.thumbnailPath = thumb.localThumbPath.getUTF8String();
        return thumb_desc;
    }
}

ODDataController* ODDataController::instance(){
	if (!mpInstance)
	{
		if (mbDestroyed)
			onDeadReference();
		else
			createInstance();
	}
	return mpInstance;
}

ODDataController::ODDataController(){
    mDataHandle = new ODDbInfo();
    mThumbHandle = new ODThumbInfo();
}

ODDataController::~ODDataController()
{
	mpInstance = 0;
	mbDestroyed = true;
    
    mDataHandle->UnInit();
    mThumbHandle->UnInit();
    
    delete mDataHandle;
    delete mThumbHandle;
    
    mDataHandle = NULL;
    mThumbHandle = NULL;
    
}

void ODDataController::createInstance()
{
	sODMutex.lock();

	if (!mpInstance)
	{
		static ODDataController instance;
		mpInstance = &instance;
	}

	sODMutex.unlock();
}

void ODDataController::killPhoenixSingleton()
{
	mpInstance->~ODDataController();	
}

void ODDataController::onDeadReference()
{
	// setup the instance shell
	createInstance();

	//mpInstance now points to raw memory used before, so create another one in that same memory
	new(mpInstance) ODDataController;

	// Queue this object for delete again
	atexit(killPhoenixSingleton);

	mbDestroyed = false;
}

void ODDataController::SetDatabasePath(const UString& path)
{
    if (mDataHandle) {
        mDataHandle->InitDB(path.getUTF8String());
    }
    if (mThumbHandle) {
        mThumbHandle->InitDB(path.getUTF8String());
    }
}

void ODDataController::GetServices(ODServiceList& services)
{
//	if (mpServiceList)
//		services = *mpServiceList;
	
}

void ODDataController::GetAllAccounts(ODAccountList& accountList)
{
    if (mDataHandle) {
        account_description_vector_t accountDescList;
        mDataHandle->GetAllAccounts(accountDescList);
        account_description_vector_t::iterator account_desc = accountDescList.begin();
        for (; account_desc != accountDescList.end(); account_desc++) {
            accountList.push_back(to_ODAccount(*account_desc));
        }
    }
}

bool ODDataController::AccountAlreadyExists(const UString& remoteId)
{
    if (mDataHandle) {
        return mDataHandle->AccountAlreadyExists(remoteId.getUTF8String());
    }
    return false;
}

UString ODDataController::GetRemoteId(const int &id)
{
    if (mDataHandle) {
        return UString::fromUtf8(mDataHandle->GetRemoteId(id).c_str());
    }
    return UString();
}

UString ODDataController::GetAccountCheckPoint(const int& id)
{
    if (mDataHandle) {
        return UString::fromUtf8(mDataHandle->GetAccountCheckPoint(id).c_str());
    }
    return UString();
}

void ODDataController::SetAccountCheckPoint(const int& id, const UString& checkPt)
{
    if (mDataHandle) {
        mDataHandle->SetAccountCheckPoint(id, checkPt.getUTF8String());
    }
}

UString ODDataController::GetAccountToken(const int& id)
{
    if (mDataHandle) {
        return UString::fromUtf8(mDataHandle->GetAccountToken(id).c_str());
    }
    return UString();
}

void ODDataController::SetAccountToken(const int& id, const UString& token)
{
    if (mDataHandle) {
        mDataHandle->SetAccountToken(id, token.getUTF8String());
    }
}

void ODDataController::GetAccountForId(const int& id, ODAccount & account)
{
    if (mDataHandle) {
        account_description_t account_desc;
        mDataHandle->GetAccountForId(id, account_desc);
        account = to_ODAccount(account_desc);
    }
}

bool ODDataController::AddAccountForService(ODAccount &account)
{
    account_description_t account_desc = to_ODAccountDescription(account);
    if(mDataHandle)
    {
        bool result = mDataHandle->AddAccountForService(account_desc);
        account.account_id = account_desc.id;
        return result;
    }
    
    return false;
}

void ODDataController::DBAccountInfoReceived(RESULT res, account_description_t account_desc, account_get_callback callback)
{
    ODAccount account = to_ODAccount(account_desc);
    callback(res, account);
}

void ODDataController::GetAccountsForService(const int& serviceid, account_get_callback callback)
{
    if(mDataHandle)
     mDataHandle->GetAccountsForService(serviceid, std::bind(&ODDataController::DBAccountInfoReceived,this,_1,_2, callback));
    else
    {
        ODAccount tempAccount;
        callback(RESULT_AccountRetreivalFailed, tempAccount);
    }
}

void ODDataController::RemoveAccount(const int& id) // removes account with id
{
}

void ODDataController::RemoveAccount(const std::string&)
{
}

long ODDataController::GetLastSyncedTimeOfAccount(const int& id)
{
    return 0;
}

bool ODDataController::UpdateOrAddMediaForAccount(ODMedia& media)
{
    media_description_t media_desc = to_ODMediaDescription(media);
    if (mDataHandle)
    {
        bool result =  mDataHandle->UpdateOrAddMediaForAccount(media_desc);
        media = to_ODMedia(media_desc);
        return result;
    }
    return true;
}

bool ODDataController::UpdateOrAddTempMediaForAccount(ODMedia& media)
{
    media_description_t media_desc = to_ODMediaDescription(media);
    if (mDataHandle)
    {
        bool result =  mDataHandle->UpdateOrAddTempMediaForAccount(media_desc);
        media = to_ODMedia(media_desc);
        return result;
    }
    return true;
}

bool ODDataController::UpdateMedia(ODMedia& media)
{
    media_description_t media_desc = to_ODMediaDescription(media);
    if (mDataHandle)
    {
        bool result =  mDataHandle->UpdateMedia(media_desc);
        media = to_ODMedia(media_desc);
        return result;
    }
    return true;
}

bool ODDataController::RemoveMedia(ODMedia& media)
{
    if (mDataHandle) {
        ID id = mDataHandle->GetMedia(media.account_id, media.file_id.getUTF8String());
        if (id != -1) {
            media.media_id = id;
            bool result = mDataHandle->RemoveMedia(id);
            mThumbHandle->RemoveThumb(media.media_id);
            return result;
        }
        else
            return false;
    }
    return true;
}

void ODDataController::DBMediaInfoReceived(RESULT res, media_description_t media_desc, media_get_callback callback)
{
    ODMedia media = to_ODMedia(media_desc);
    callback(res, media, false);
}

void ODDataController::GetNewlyUpdatedMedia(media_get_callback callback)
{
    if(mDataHandle)
        mDataHandle->GetNewlyUpdatedMedia(std::bind(&ODDataController::DBMediaInfoReceived,this,_1,_2, callback));
}

void ODDataController::GetMediaForAccount(const int& id, media_get_callback callback)
{
    if(mDataHandle)
        mDataHandle->GetMediaForAccount(id, std::bind(&ODDataController::DBMediaInfoReceived,this,_1,_2, callback));
}

UString ODDataController::GetPathForFileId(const UString& fileId)
{
    std::string path;
    if (mDataHandle) {
        path = mDataHandle->GetPathForFileId(fileId.getUTF8String());
    }
    return UString::fromUtf8(path.c_str());
}

ODMedia ODDataController::GetMediaForFileId(const int& accountId, const UString& fileId)
{
    ODMedia media;
    if (mDataHandle) {
        media = to_ODMedia(mDataHandle->GetMediaForFileId(accountId, fileId.getUTF8String()));
    }
    return media;
}

ODMedia ODDataController::GetMediaForId(const ID& Id)
{
    ODMedia media;
    if (mDataHandle) {
        media = to_ODMedia(mDataHandle->GetMediaForId(Id));
    }
    return media;
}

bool ODDataController::UpdateOrAddThumbnailForMedia(ODThumb& thumb)
{
    thumb_description_t thumb_desc = to_ODThumbDescription(thumb);
    if (mThumbHandle)
    {
        bool result =  mThumbHandle->UpdateOrAddThumbnailForMedia(thumb_desc);
        thumb = to_ODThumb(thumb_desc);
        return result;
    }
    return true;
}

void ODDataController::DBThumbnailInfoReceived(RESULT res, thumb_description_t thumb_desc, thumbnail_get_callback callback)
{
    ODThumb thumb = to_ODThumb(thumb_desc);
    callback(res, thumb);
}

void ODDataController::GetThumbnailForMedia(const ID& mediaId, int width, int height,
                                            thumbnail_get_callback& callback)
{
    if (mThumbHandle)
        mThumbHandle->GetThumbForMedia(mediaId, width, height, std::bind(&ODDataController::DBThumbnailInfoReceived, this,
                                                                         _1, _2, callback));
}

bool ODDataController::IsThumbnailEmpty(const ID& mediaId, int width, int height)
{
    if (mThumbHandle) {
        return mThumbHandle->IsThumbnailEmpty(mediaId, width, height);
    }
    return true;
}

void ODDataController::GetMediaByProperty(const std::string& relativePath, const std::string& searchString, media_get_callback callback, int accountId)
{
    if (mDataHandle) {
        mDataHandle->StopCurrentRunningQuery();
        mDataHandle->GetMediaByProperties(relativePath, searchString, accountId, [this, callback](RESULT res, media_description_t media_desc)
                                        {
                                            ODMedia media = to_ODMedia(media_desc);
                                            callback(res, media, false);
                                        });
    }
}

void ODDataController::GetMediaByMimeType(const std::string& mimeType, media_get_callback callback, int accountId)
{
    if (mDataHandle) {
        mDataHandle->GetMediaByMimeType(mimeType, accountId, [this, callback](RESULT res, media_description_t media_desc)
                                        {
                                            ODMedia media = to_ODMedia(media_desc);
                                            callback(res, media,false);
                                        });
    }
}

void ODDataController::GetMediaListForParent(const ID& parentId, const int& accountId, ODMediaList& childList)
{
    if (mDataHandle) {
        media_description_vector_t mediaList;
        mDataHandle->GetMediaListForParent(parentId, accountId, mediaList);
        media_description_vector_t::iterator media_desc = mediaList.begin();
        for (; media_desc != mediaList.end(); media_desc++) {
            childList.push_back(to_ODMedia(*media_desc));
        }
    }
}

int ODDataController::GetMediaDownloadStatus(const ID& mediaId)
{
    if (mDataHandle) {
        return mDataHandle->GetMediaDownloadStatus(mediaId);
    }
    return 0;
}

UString ODDataController::GetMediaDownloadPath(const ID& mediaId)
{
    if (mDataHandle) {
        return UString::fromUtf8(mDataHandle->GetMediaDownloadPath(mediaId).c_str());
    }
    return UString();
}

UString ODDataController::GetMediaDownloadRevision(const ID& mediaId)
{
    if (mDataHandle) {
        return UString::fromUtf8(mDataHandle->GetMediaDownloadRevision(mediaId).c_str());
    }
    return UString();
}

void ODDataController::SetMediaDownloadStatus(const ID& id, const int& status)
{
    if (mDataHandle) {
        if (mDataHandle->GetMediaDownloadStatus(id) == -1) {
            mDataHandle->AddMediaMetadata(id, "downloadStatus", status);
        }
        else {
            mDataHandle->UpdateMediaMetadata(id, "downloadStatus", status);
        }
    }
}

void ODDataController::SetMediaDownloadPath(const ID& id, const UString& path)
{
    if (mDataHandle) {
        if (mDataHandle->GetMediaDownloadPath(id).empty()) {
            mDataHandle->AddMediaMetadata(id, "downloadPath", path.getUTF8String());
        }
        else {
           mDataHandle->UpdateMediaMetadata(id, "downloadPath", path.getUTF8String());
        }
    }
}

void ODDataController::SetMediaDownloadRevision(const ID& id, const UString& revision)
{
    if (mDataHandle) {
        if (mDataHandle->GetMediaDownloadRevision(id).empty()) {
            mDataHandle->AddMediaMetadata(id, "downloadRevision", revision.getUTF8String());
        }
        else {
            mDataHandle->UpdateMediaMetadata(id, "downloadRevision", revision.getUTF8String());
        }
    }
}

void ODDataController::SetMediaDownloadDetails(const ID& id, const UString& path, const UString& revision)
{
    if (mDataHandle) {
        if (mDataHandle->GetMediaDownloadRevision(id).empty()) {
            mDataHandle->AddMediaDownloadDetails(id, path.getUTF8String(), revision.getUTF8String());
        }
        else {
            mDataHandle->UpdateMediaDownloadDetails(id, path.getUTF8String(), revision.getUTF8String());
        }
    }
}

void ODDataController::SetMediaAsFavorite(const ID& id)
{
    if (mDataHandle)
        mDataHandle->SetMediaAsFavorite(id, true);
}

void ODDataController::RemoveMediaFromFavorite(const ID& id)
{
    if (mDataHandle)
        mDataHandle->SetMediaAsFavorite(id, false);
}

void ODDataController::GetFavoriteMedia(media_get_callback callback, int accountId)
{
    if (mDataHandle) {
        mDataHandle->GetFavoriteMedia(accountId, [this, callback](RESULT res, media_description_t media_desc)
                                        {
                                            ODMedia media = to_ODMedia(media_desc);
                                            callback(res, media, false);
                                        });
    }
}

bool ODDataController::IsMediaFavorite(const ID& id)
{
    if (mDataHandle) {
        return mDataHandle->IsMediaFavorite(id) == 1;
    }
    return false;
}


void ODDataController::AddToRecent(const ID& id)
{
    if (mDataHandle) {
        mDataHandle->AddRecentItem(id);
    }
}

void ODDataController::GetRecentItems(ODMediaList& recentList, int limit)
{
    std::vector<ID> idList;
    if (mDataHandle) {
        mDataHandle->GetRecents(idList);
    }

    int size = (limit < idList.size()) ? limit : idList.size();
    for (int i=0; i<size; i++) {
        ODMedia media = GetMediaForId(idList[i]);
        recentList.push_back(media);
    }
}

