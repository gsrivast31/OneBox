#ifndef ONEDRIVE_ODINFO_H
#define ONEDRIVE_ODINFO_H
#include <sqlite3.h>
#include "Types.h"
#include <string>
#include <vector>
#include <functional>

namespace NewGroup
{
    using namespace NewGroup::Core;
    
	struct account_description_t
	{
		account_description_t()
			: id(0)
		{
		}
		int id;
		int service_id;
        std::string remote_id;
		std::string name;
        std::string token;
        std::string checkpoint;
		bool status;
	};
    
    struct media_description_t
    {
        media_description_t()
            : id(0)
        {
        }
        ID id;
        int account_id;
        ID parent_id;
        std::string file_id;
        std::string parent_file_id;
        std::string full_path;
        std::string mime_type;
        bool is_folder;
        unsigned long long size;
        std::string icon;
        std::string revId;
        std::string file_name;
        unsigned long long last_mod_date;
    };
    
    namespace mime_type
    {
        extern const std::string image;
        extern const std::string video;
        extern const std::string document;
        extern const std::string pdf;
        extern const std::string contact;
        extern const std::string folder;
        extern const std::string unknown;
    };
    
    namespace metadata_type
    {
        extern const std::string integer_type;
        extern const std::string string_type;
        extern const std::string blob_type;
        extern const std::string date_type;
    };
    
    namespace metadata_description
    {
        extern const std::string downloadStatus;
        extern const std::string downloadPath;
        extern const std::string downloadRevision;
        extern const std::string isFavorite;
        extern const std::string size;
    };
    
	typedef std::vector<account_description_t> account_description_vector_t;
    typedef std::vector<media_description_t> media_description_vector_t;
    
	typedef std::function<void (RESULT, account_description_t)> account_progress_callback;
    typedef std::function<void (RESULT, media_description_t)> media_progress_callback;

    class ODDbInfo
	{
	public:
		ODDbInfo();
        ~ODDbInfo() {};

		void InitDB(const std::string&);
		void UnInit();
		void CreateTables();
        
        /* Temporary table related calls */
        bool UpdateOrAddTempMediaForAccount(media_description_t&);
        void GetNewlyUpdatedMedia(media_progress_callback cb);
        
        /* Account Related DB calls */
        void GetAllAccounts(account_description_vector_t&);
        bool AddAccountForService(account_description_t&);
        bool AccountAlreadyExists(const std::string&);
        std::string GetRemoteId(const int&);
		void GetAccountsForService(int serviceId, account_progress_callback cb);
        std::string GetAccountCheckPoint(const int&);
        void SetAccountCheckPoint(const int&, const std::string&);
        std::string GetAccountToken(const int&);
        void SetAccountToken(const int&, const std::string&);
        void GetAccountForId(const int&, account_description_t&);
        
		void GetMediaForAccount(int id, media_progress_callback cb);

        /* Media Related DB calls */
        bool UpdateOrAddMediaForAccount(media_description_t&);
        ID GetMedia(const int accountId, const std::string& fileId);
        ID GetMediaID(const std::string&);
        bool AddMediaForAccount(media_description_t&);
        bool UpdateMedia(media_description_t&);
        bool RemoveMedia(const ID&);
        std::string GetPathForFileId(const std::string&);
        media_description_t GetMediaForFileId(const int& accountId, const std::string&);
        media_description_t GetMediaForId(const ID&);
        void GetMediaListForParent(const ID& parentId, const int& accountId, media_description_vector_t& childList);
        int GetMediaDownloadStatus(const ID&);
        std::string GetMediaDownloadPath(const ID&);
        std::string GetMediaDownloadRevision(const ID&);
        
        void AddMediaMetadata(const ID&, const std::string& identifier, const std::string& value);
        void AddMediaMetadata(const ID&, const std::string& identifier, const int& value);
        
        void UpdateMediaMetadata(const ID&, const std::string& identifier, const std::string& value);
        void UpdateMediaMetadata(const ID&, const std::string& identifier, const int& value);
        
         /* Search Related DB calls */
        void GetMediaByMimeType(const std::string&, int accountId, media_progress_callback);
        void GetMediaByProperties(const std::string &relativePath,const std::string&, int accountId, media_progress_callback);
        void GetFavoriteMedia(int accountId, media_progress_callback);
        
        void SetMediaAsFavorite(const ID&, bool set = true);
        int  IsMediaFavorite(const ID&);
        
        void UpdateMediaDownloadDetails(const ID&, const std::string& downloadPath, const std::string& downloadRevision);
        void AddMediaDownloadDetails(const ID&, const std::string& downloadPath, const std::string& downloadRevision);

        void AddRecentItem(const ID&);
        void GetRecents(std::vector<ID>&);
        
        /* Misc */
        void StopCurrentRunningQuery();

	private:
        bool PrepareDB(const char *query, sqlite3_stmt** statement);
        bool RunQuery(const char* query);

        void CreateTemporaryTable();
        ID GetTempMediaID(const std::string&);
        bool AddTempMediaForAccount(media_description_t&);
        bool UpdateTempMedia(media_description_t&);
        bool RemoveTempMedia(const ID&);
        void GetAllTempMedia(media_description_vector_t& mediaList);
        media_description_t GetTempMediaForFileId(const int& accountId, const std::string&);
        void CommitTempAdditions(media_description_t&, media_progress_callback cb);
        
        int AddMetadataDescriptions(std::string identifier, std::string data_type);

        int GetDescriptionIdByIdentifier(const std::string& identifier, const std::string& data_type);
        int AddMediaStringMetadata(const std::string& identifier, const std::string& value);
        int AddMediaIntegerMetadata(const std::string& identifier, const int& value);
        
        void AddMetadata(const ID&, const int&);
        
        void CreateFirstAutoId();
        void UpdateAutoId();
        int GetNewRecordAutoId();
        
        void BeginTransaction();
        void EndTransaction();
        
        std::string mDatabasePath;
		sqlite3* mDBHandle;
        bool mStopCurrentQuery;
        
        int m_current_auto_id;
        bool m_requested_new_auto_id_this_transaction;
        bool m_have_open_transaction;
        bool m_auto_id_generated;
	};

} //namespace ODCore

#endif