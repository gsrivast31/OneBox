//
//  DataController.h
//  DataStorage
//
//  Created by OneDrive on 12/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef DataStorage_DataController_h
#define DataStorage_DataController_h

#include "Types.h"
#include <string>
#include "UString.h"
#include "ODThumbInfo.h"
#include "ODInfo.h"

using namespace std;

using namespace NewGroup;
using namespace NewGroup::Core;

enum eSiblingOrderPolicy {
    kOrderByName,
    kOrderByDate,
    kOrderBySize,
    kOrderByType
};

struct ODService
{
	int service_id;
    std::string service_name;
    std::string service_icon;
};

struct ODAccount
{
	int account_id;
	int account_service_id;
    UString account_remote_id;
	UString account_name;
	UString account_token;
    UString icon_path;
	bool account_status;
    UString checkpoint;
};

struct ODMedia
{
	ID media_id;
	int account_id;
    ID parent_id;
	UString file_id;
    UString parent_file_id;
	UString full_path;
    UString file_name;
	UString mime_type;
    bool is_folder;
    unsigned long long size;
    UString icon;
    UString revisionId;
    DateTime mod_date;
};

struct ODThumb
{
    ID thumb_id;
    ID media_id;
    UString localThumbPath;
    int width;
    int height;
};

typedef std::vector<ODService> ODServiceList;
typedef std::vector<ODAccount> ODAccountList;
typedef std::vector<ODMedia> ODMediaList;
typedef std::vector<ODThumb> ODThumbList;

typedef const ODMedia& ConstMediaRef;

class ODDataController
{
    typedef std::function<void (RESULT, ODAccount)> account_get_callback;
    typedef std::function<void (RESULT, ODMedia, bool deleted)> media_get_callback;
    typedef std::function<void (RESULT, ODThumb)> thumbnail_get_callback;
    typedef std::function<void (RESULT, ODMedia)> media_search_callback;
    
public:
	static ODDataController* instance();

    void SetDatabasePath(const UString&);
    void GetServices(ODServiceList&);

	//ODAccount AddAccountForService(const ODService&, const std::string&);
	bool AddAccountForService(ODAccount &);
    void GetAccountsForService(const int&, account_get_callback);

    void GetAllAccounts(ODAccountList& accountList);
    
    /* Account Related Calls */
    bool AccountAlreadyExists(const UString&);
    UString GetRemoteId(const int&);
    UString GetAccountCheckPoint(const int&);
    void SetAccountCheckPoint(const int&, const UString&);
    UString GetAccountToken(const int&);
    void SetAccountToken(const int&, const UString&);
    void GetAccountForId(const int&, ODAccount &);
    void RemoveAccount(const int& id); // removes account with id
	void RemoveAccount(const std::string&);
    long GetLastSyncedTimeOfAccount(const int& id);
    
    /* Media Related Calls */
    bool UpdateOrAddMediaForAccount(ODMedia&);
    bool UpdateOrAddTempMediaForAccount(ODMedia&);
    bool UpdateMedia(ODMedia&);
    bool RemoveMedia(ODMedia&);
    void GetMediaForAccount(const int& id, media_get_callback);
    UString GetPathForFileId(const UString&);
    ODMedia GetMediaForFileId(const int& accountId, const UString&);
    ODMedia GetMediaForId(const ID&);
    void GetMediaListForParent(const ID&, const int&, ODMediaList&);

    void GetNewlyUpdatedMedia(media_get_callback);
    
    /* Thumbnail Related Calls */
    bool UpdateOrAddThumbnailForMedia(ODThumb& thumb);
    void GetThumbnailForMedia(const ID&, int, int, thumbnail_get_callback&);
    bool IsThumbnailEmpty(const ID&, int, int);
    
    void GetMediaByMimeType(const std::string&, media_get_callback, int accountId = -1);
    void GetMediaByProperty(const std::string&, const std::string&, media_get_callback, int accountId = -1);
    
    int GetMediaDownloadStatus(const ID& mediaId);
    UString GetMediaDownloadPath(const ID& mediaId);
    UString GetMediaDownloadRevision(const ID& mediaId);
    
    void SetMediaDownloadStatus(const ID& id, const int& status);
    void SetMediaDownloadPath(const ID& id, const UString& path);
    void SetMediaDownloadRevision(const ID& id, const UString& revision);
    void SetMediaDownloadDetails(const ID& id, const UString& path, const UString& revision);
    
    void SetMediaAsFavorite(const ID& id);
    void RemoveMediaFromFavorite(const ID& id);
    void GetFavoriteMedia(media_get_callback, int accountId = -1);
    bool IsMediaFavorite(const ID& id);
    
    void AddToRecent(const ID&);
    void GetRecentItems(ODMediaList&, int limit);
    
private:
	ODDataController();
	ODDataController(const ODDataController&);
	virtual ~ODDataController();
	const ODDataController& operator=(const ODDataController&);

    void DBAccountInfoReceived(RESULT res, account_description_t, account_get_callback);
    void DBMediaInfoReceived(RESULT res, media_description_t, media_get_callback);
    void DBThumbnailInfoReceived(RESULT res, thumb_description_t, thumbnail_get_callback);
    
	// Singleton management functions
	static void createInstance();
	static void killPhoenixSingleton();
	static void onDeadReference();

private:
	/* Singleton instance */
	static ODDataController* mpInstance;
	/* Flag to determine if we've been killed */
	static bool mbDestroyed;
    ODDbInfo* mDataHandle;
    ODThumbInfo* mThumbHandle;
	
};

#endif
