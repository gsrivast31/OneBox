#include "ODInfo.h"
#include <functional>
#import <Foundation/Foundation.h>

namespace NewGroup
{
    const std::string mime_type::image = "image/*";
    const std::string mime_type::video = "video/*";
    const std::string mime_type::document = "document/*";
    const std::string mime_type::pdf = "pdf/*";
    const std::string mime_type::contact = "contact/*";
    const std::string mime_type::folder = "folder/*";
    const std::string mime_type::unknown = "unknown/*";
    const int busyRetryTimeout = 100; // I think its more than enough. Revisit.
    
    const std::string metadata_type::integer_type = "integer_type";
    const std::string metadata_type::string_type = "string_type";
    const std::string metadata_type::date_type = "date_type";
    const std::string metadata_type::blob_type = "blob_type";
    
    const std::string metadata_description::downloadStatus = "downloadStatus";
    const std::string metadata_description::downloadPath = "downloadPath";
    const std::string metadata_description::downloadRevision = "downloadRevision";
    const std::string metadata_description::isFavorite = "isFavorite";
    const std::string metadata_description::size = "size";
    
	static int sqlite_registered_cb(void* this_void_ptr)
	{
        return SQLITE_OK;
	}
	
	ODDbInfo::ODDbInfo()
		:mDBHandle(NULL),
        mStopCurrentQuery(false)
	{
	}

	void ODDbInfo::InitDB(const std::string& databaseDir )
	{
        m_current_auto_id = -1;
        m_auto_id_generated = false;
        
		mDatabasePath = databaseDir + "/db.cache";
		int err_code = sqlite3_open_v2(mDatabasePath.c_str(), &mDBHandle,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX,NULL);
		if (err_code != SQLITE_OK)
		{
			std::string err = sqlite3_errmsg(mDBHandle);
			sqlite3_close(mDBHandle);
            return;
		}
		
		CreateTables();
        
        if (GetNewRecordAutoId() == 0) {
            std::string query = "BEGIN TRANSACTION";
            RunQuery(query.c_str());
            m_have_open_transaction = true;
            m_requested_new_auto_id_this_transaction = false;
            
            std::string insertQuery;
            insertQuery = "INSERT INTO _auto_id_table VALUES (?)";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, m_current_auto_id);
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
            query = "END TRANSACTION";
            RunQuery(query.c_str());
            m_have_open_transaction = false;
        }

	}

	void ODDbInfo::CreateTables()
	{
        if(!mDBHandle)
            return;
        try {
            std::string query;
            query = "CREATE TABLE IF NOT EXISTS _auto_id_table (last_assigned_id INTEGER PRIMARY KEY);";
           
            RunQuery(query.c_str());
            
            query = "CREATE TABLE IF NOT EXISTS account_info (";
            query += "id INTEGER PRIMARY KEY, ";
            query += "service_id INTEGER NOT NULL, ";
            query += "remote_id VARCHAR(255) UNIQUE NOT NULL, ";
            query += "name VARCHAR(255) NOT NULL,";
            query += "token VARCHAR(255),";
            query += "checkpoint VARCHAR(255), ";
            query += "status INT";
            query += ");";
            
            RunQuery(query.c_str());
            
            query  = "CREATE TABLE IF NOT EXISTS media_info (";
            query += "id INTEGER PRIMARY KEY, ";
            query += "account_id INTEGER NOT NULL, ";
            query += "parent_id INTEGER NOT NULL, ";
            query += "file_id VARCHAR(255) NOT NULL,";
            query += "parent_file_id VARCHAR(255) NOT NULL,";
            query += "full_path VARCHAR(255) NOT NULL,";
            query += "mime_type VARCHAR(255) NOT NULL,";
            query += "is_folder INTEGER NOT NULL,";
            query += "size BIGINT NOT NULL,";
            query += "icon VARCHAR(255),";
            query += "asset_name VARCHAR(255) NOT NULL,";
            query += "revision_id VARCHAR(255) NOT NULL,";
            query += "last_mod_date BIGINT";
            query += ");";
            
            RunQuery(query.c_str());
            
            query = "CREATE TABLE IF NOT EXISTS media_metadata_info (";
            query += "id INTEGER NOT NULL, ";
            query += "metadata_id INTEGER NOT NULL";
            query += ");";
            
            RunQuery(query.c_str());
            
            query = "CREATE TABLE IF NOT EXISTS metadata_string_info (";
            query += "id INTEGER PRIMARY KEY, ";
            query += "description_id INTEGER NOT NULL, ";
            query += "value VARCHAR(255)";
            query += ");";
            
            RunQuery(query.c_str());
            
            query = "CREATE TABLE IF NOT EXISTS metadata_integer_info (";
            query += "id INTEGER PRIMARY KEY, ";
            query += "description_id INTEGER NOT NULL, ";
            query += "value BIGINT";
            query += ");";
            
            RunQuery(query.c_str());
            
            query = "CREATE TABLE IF NOT EXISTS metadata_description_info (";
            query += "id INTEGER PRIMARY KEY, ";
            query += "identifier VARCHAR(255), ";
            query += "data_type VARCHAR(32)";
            query += ");";
            
            RunQuery(query.c_str());
            
            query = "CREATE TABLE IF NOT EXISTS recent_info (";
            query += "id INTEGER PRIMARY KEY, ";
            query += "media_id INTEGER NOT NULL";
            query += ");";
            
            RunQuery(query.c_str());
            
            CreateTemporaryTable();
            
            
      } catch (...) {
            
        }
	}
    
    void ODDbInfo::CreateTemporaryTable()
    {
        if(!mDBHandle)
            return;
        try {
            std::string query;

            query  = "CREATE TEMPORARY TABLE IF NOT EXISTS temp_media_info (";
            query += "id INTEGER PRIMARY KEY, ";
            query += "account_id INTEGER NOT NULL, ";
            query += "parent_id INTEGER NOT NULL, ";
            query += "file_id VARCHAR(255) NOT NULL,";
            query += "parent_file_id VARCHAR(255) NOT NULL,";
            query += "full_path VARCHAR(255) NOT NULL,";
            query += "mime_type VARCHAR(255) NOT NULL,";
            query += "is_folder INTEGER NOT NULL,";
            query += "size BIGINT NOT NULL,";
            query += "icon VARCHAR(255),";
            query += "asset_name VARCHAR(255) NOT NULL,";
            query += "revision_id VARCHAR(255) NOT NULL,";
            query += "last_mod_date BIGINT";
            query += ");";
        
            RunQuery(query.c_str());
        }
        catch(...){
            
        }
    }
    
    void ODDbInfo::UpdateAutoId()
    {
        if (!m_requested_new_auto_id_this_transaction)
            return;
        
        try {
            std::string insertQuery;
            
            insertQuery = "UPDATE _auto_id_table SET last_assigned_id = ?";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, m_current_auto_id);
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
        } catch (...) {
        }
    }
    
    int ODDbInfo::GetNewRecordAutoId()
    {
        if (!m_requested_new_auto_id_this_transaction) {
            try{
                std::string query = "SELECT last_assigned_id FROM _auto_id_table";
                sqlite3_stmt* statement;
                bool rc = PrepareDB(query.c_str(), &statement);
                if (rc){
                    while(true){
                        int result = sqlite3_step(statement);
                        if (result == SQLITE_ROW){
                            m_current_auto_id = sqlite3_column_int(statement, 0);
                            m_auto_id_generated = true;
                            m_requested_new_auto_id_this_transaction = true;
                        }
                        else
                            break;
                    }
                    sqlite3_finalize(statement);
                }
            }
            catch(...)
            {
                
            }
        }
        m_current_auto_id++;
        return m_current_auto_id;
    }
    
    void ODDbInfo::BeginTransaction()
    {
       // if (m_have_open_transaction)
       //     throw std::logic_error("There is already a transaction opened. Transactions cannot be nested");
        std::string query = "BEGIN TRANSACTION";
        RunQuery(query.c_str());
        m_have_open_transaction = true;
        m_requested_new_auto_id_this_transaction = false;
    }
    
    void ODDbInfo::EndTransaction()
    {
        UpdateAutoId();
        std::string query = "END TRANSACTION";
        RunQuery(query.c_str());
        m_have_open_transaction = false;
    }
    
    int ODDbInfo::AddMetadataDescriptions(std::string identifier, std::string data_type)
    {
        int desc_id = -1;
        try {
            BeginTransaction();
            std::string insertQuery;
            insertQuery += "INSERT INTO metadata_description_info (\"id\", \"identifier\", \"data_type\") ";
            insertQuery += "VALUES (?, ?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                desc_id = GetNewRecordAutoId();
                sqlite3_bind_int(statement, 1, desc_id);
                sqlite3_bind_text(statement, 2, identifier.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 3, data_type.c_str(),-1,NULL);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
            
        } catch (...) {
        }
        
        return desc_id;
    }
	
	void ODDbInfo::UnInit()
	{
		sqlite3_close(mDBHandle);
	}
    
    void ODDbInfo::GetAllAccounts(account_description_vector_t& accountDescList)
    {
		std::string	query = "SELECT * FROM account_info";
		int result = 0;
        
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
			while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					account_description_t account;
					account.id = sqlite3_column_int(statement, 0);
					account.service_id = sqlite3_column_int(statement, 1);
                    account.remote_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
					account.name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    account.token = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    account.checkpoint = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
					account.status = sqlite3_column_int(statement, 6);
				
                    accountDescList.push_back(account);
                }
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
        
    }

	bool ODDbInfo::AddAccountForService(account_description_t& account)
	{
        bool success = true;
        try {
            BeginTransaction();
            
            std::string insertQuery;
            insertQuery += "INSERT INTO account_info (\"id\", \"service_id\", \"remote_id\", \"name\", \"token\",\"checkpoint\", \"status\") ";
            insertQuery += "VALUES (?, ?, ?, ?, ?, ?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                account.id = GetNewRecordAutoId();
                sqlite3_bind_int(statement, 1, account.id);
                sqlite3_bind_int(statement, 2, account.service_id);
                sqlite3_bind_text(statement, 3, account.remote_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 4, account.name.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 5, account.token.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 6, account.checkpoint.c_str(), -1, NULL);
                sqlite3_bind_int(statement, 7, account.status);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);				
            NSLog(@"%s", error.c_str());
            
            EndTransaction();
            
        } catch (...) {
            success = false;
        }
        
        return success;
	}
	
    bool ODDbInfo::AccountAlreadyExists(const std::string& remoteId)
    {
        bool success = false;
        try{
            std::string query = "SELECT id FROM account_info WHERE remote_id=?";
            sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_text(statement, 1, remoteId.c_str(),-1,NULL);
                int result = sqlite3_step(statement);
                if (result == SQLITE_ROW)
                    success = true;
                sqlite3_finalize(statement);
            }
            else
                return false;
        }
        catch(...)
        {
            success  = false;
        }
        
        return success;
    }

    std::string ODDbInfo::GetRemoteId(const int &id)
    {
        std::string remoteId;
        try{
            std::string query = "SELECT remote_id FROM account_info WHERE id=?";
            sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, id);
                int result = sqlite3_step(statement);
                if (result == SQLITE_ROW)
                    remoteId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
                sqlite3_finalize(statement);
            }
        }
        catch(...)
        {
        }
        
        return remoteId;
    }
    
	void ODDbInfo::GetAccountsForService(int serviceId, account_progress_callback cb)
	{
		sqlite3_progress_handler(mDBHandle,
									 100,
								 	 &sqlite_registered_cb,
								 	 NULL);
									 
		std::string	query = "SELECT * FROM account_info WHERE service_id=?";
		int result = 0;

        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
			sqlite3_bind_int(statement, 1, serviceId);
			while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					account_description_t account;
					account.id = sqlite3_column_int(statement, 0);
					account.service_id = sqlite3_column_int(statement, 1);
                    account.remote_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
					account.name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    account.token = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    account.checkpoint = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
					account.status = sqlite3_column_int(statement, 6);
					
					cb(RESULT_NoErr, account);
				}
				else 
					break;
			}
			sqlite3_finalize(statement);
		}

		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
	}
	
    void ODDbInfo::GetMediaForAccount(int id, media_progress_callback cb)
    {
		sqlite3_progress_handler(mDBHandle,
                                 100,
                                 &sqlite_registered_cb,
                                 NULL);
        
		std::string	query = "SELECT * FROM media_info WHERE account_id=?";
		int result = 0;
        
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
			sqlite3_bind_int(statement, 1, id);
            int count = 0;
			while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media_description_t media;
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
                    cb(RESULT_NoErr, media);
                    ++count;
				}
				else
					break;
			}
            
            if(!count){
                media_description_t media;
                media.account_id = id;
                cb(RESULT_NoMediaChanges, media);
            }
                
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
    }

    std::string ODDbInfo::GetAccountCheckPoint(const int& id)
    {
        std::string checkPt;
        try{
            std::string query = "SELECT checkpoint FROM account_info WHERE id=?";
            sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, id);
                int result = sqlite3_step(statement);
                if (result == SQLITE_ROW)
                    checkPt = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
                sqlite3_finalize(statement);
            }
        }
        catch(...)
        {
        }
        
        return checkPt;
    }
    
    void ODDbInfo::SetAccountCheckPoint(const int& id, const std::string& checkPt)
    {
        try {
            BeginTransaction();
            
            std::string updateQuery;
            updateQuery += "UPDATE account_info SET checkpoint = ? ";
            updateQuery += "WHERE id = ?;";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(updateQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_text(statement, 1, checkPt.c_str(),-1,NULL);
                sqlite3_bind_int(statement, 2, id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
            EndTransaction();
        }
        catch (...) {
        }
    }

    std::string ODDbInfo::GetAccountToken(const int& id)
    {
        std::string token;
        try{
            std::string query = "SELECT token FROM account_info WHERE id=?";
            sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, id);
                int result = sqlite3_step(statement);
                if (result == SQLITE_ROW)
                    token = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
                sqlite3_finalize(statement);
            }
        }
        catch(...)
        {
        }
        
        return token;
    }
    
    void ODDbInfo::SetAccountToken(const int& id, const std::string& token)
    {
        try {
            BeginTransaction();
            
            std::string updateQuery;
            updateQuery += "UPDATE account_info SET token = ? ";
            updateQuery += "WHERE id = ?;";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(updateQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_text(statement, 1, token.c_str(),-1,NULL);
                sqlite3_bind_int(statement, 2, id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
            EndTransaction();
        }
        catch (...) {
        }
    }
    
    void ODDbInfo::GetAccountForId(const int& id, account_description_t& account)
    {
        try {
            std::string query = "SELECT * FROM account_info WHERE id=?";
            sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, id);
                int result = sqlite3_step(statement);
                if (result == SQLITE_ROW){
					account.id = sqlite3_column_int(statement, 0);
					account.service_id = sqlite3_column_int(statement, 1);
                    account.remote_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
					account.name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    account.token = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    account.checkpoint = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
					account.status = sqlite3_column_int(statement, 6);
                }
                sqlite3_finalize(statement);
            }
        } catch (...) {
            
        }
    }

    bool ODDbInfo::UpdateOrAddMediaForAccount(media_description_t& media)
    {
        media_description_t dbMedia = GetMediaForFileId(media.account_id, media.file_id);
        ID id = dbMedia.id;
        ID parentid = GetMediaForFileId(media.account_id, media.parent_file_id).id;
        
        if (id > 0) {
            media.id = id;
            if (dbMedia.revId != media.revId) {
                return UpdateMedia(media);
            }
            else
            {
                media = dbMedia;
                return true;
            }
        }
        else if (media.parent_file_id == "/" || parentid > 0) {
            if (parentid > 0) {
                media.parent_id = parentid;
            }
            return AddMediaForAccount(media);
        }
        return false;
    }

    bool ODDbInfo::AddMediaForAccount(media_description_t& media)
    {
        bool success = true;
        try {
            BeginTransaction();
            std::string insertQuery;
            
            if (media.file_name.empty()) {
                int index =  media.full_path.find_last_of("/\\"); // Find index of last slash if it exists
                if(index != std::string::npos)
                    media.file_name = media.full_path.substr(index+1);
            }
            
            insertQuery += "INSERT INTO media_info (\"id\", \"account_id\", \"parent_id\", \"file_id\", \"parent_file_id\", \"full_path\", \"mime_type\", \"is_folder\", \"size\", \"icon\", \"asset_name\", \"revision_id\", \"last_mod_date\")";
            insertQuery += "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                media.id = GetNewRecordAutoId();
                sqlite3_bind_int64(statement, 1, media.id);
                sqlite3_bind_int64(statement, 2, media.account_id);
                sqlite3_bind_int64(statement, 3, media.parent_id);
                sqlite3_bind_text(statement, 4, media.file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 5, media.parent_file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 6, media.full_path.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 7, media.mime_type.c_str(),-1,NULL);
                sqlite3_bind_int(statement, 8, media.is_folder);
                sqlite3_bind_int64(statement, 9, media.size);
                sqlite3_bind_text(statement, 10, media.icon.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 11, media.file_name.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 12, media.revId.c_str(), -1, NULL);
                sqlite3_bind_int64(statement, 13, media.last_mod_date);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
        }
        catch (...) {
            success = false;
        }
        
       return success;
    }

    bool ODDbInfo::AddTempMediaForAccount(media_description_t& media)
    {
        bool success = true;
        try {
            BeginTransaction();
            std::string insertQuery;
            
            if (media.file_name.empty()) {
                int index =  media.full_path.find_last_of("/\\"); // Find index of last slash if it exists
                if(index != std::string::npos)
                    media.file_name = media.full_path.substr(index+1);
            }
            
            insertQuery += "INSERT INTO temp_media_info (\"id\", \"account_id\", \"parent_id\", \"file_id\", \"parent_file_id\", \"full_path\", \"mime_type\", \"is_folder\", \"size\", \"icon\", \"asset_name\", \"revision_id\", \"last_mod_date\")";
            insertQuery += "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                media.id = GetNewRecordAutoId();
                sqlite3_bind_int64(statement, 1, media.id);
                sqlite3_bind_int64(statement, 2, media.account_id);
                sqlite3_bind_int64(statement, 3, media.parent_id);
                sqlite3_bind_text(statement, 4, media.file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 5, media.parent_file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 6, media.full_path.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 7, media.mime_type.c_str(),-1,NULL);
                sqlite3_bind_int(statement, 8, media.is_folder);
                sqlite3_bind_int64(statement, 9, media.size);
                sqlite3_bind_text(statement, 10, media.icon.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 11, media.file_name.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 12, media.revId.c_str(), -1, NULL);
                sqlite3_bind_int64(statement, 13, media.last_mod_date);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
        }
        catch (...) {
            success = false;
        }
        
        return success;
    }

    ID ODDbInfo::GetTempMediaID(const std::string& fullpath) 
    {
        ID mediaId = -1;
        try {
            std::string query = "SELECT id FROM temp_media_info WHERE full_path=?";
           	sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_text(statement, 1,  fullpath.c_str(), -1, NULL);
                int result = sqlite3_step(statement);
                if(result == SQLITE_ROW)
                    mediaId = sqlite3_column_int64(statement, 0);
                sqlite3_finalize(statement);
            }
        } catch (...) {
        }
        return mediaId;
    }
    
    media_description_t ODDbInfo::GetTempMediaForFileId(const int& accountId, const std::string& fileId)
    {
        std::string	query = "SELECT * FROM temp_media_info WHERE account_id=? AND file_id=?";
        media_description_t media;
		int result = 0;
        
		sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int(statement, 1, accountId);
            sqlite3_bind_text(statement, 2, fileId.c_str(),-1,NULL);
            
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
        
        return media;

    }

    bool ODDbInfo::UpdateTempMedia(media_description_t& media)
    {
        bool success = true;
        try {
            BeginTransaction();
            std::string updateQuery;
            updateQuery += "UPDATE temp_media_info SET parent_id = ?, file_id = ?, parent_file_id = ?, full_path = ?, mime_type = ?, size = ?, icon = ?, asset_name = ?, revision_id = ?, last_mod_date = ? ";
            updateQuery += "WHERE id = ?;";
            
           	sqlite3_stmt* statement;
            bool rc = PrepareDB(updateQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_int64(statement, 1, media.parent_id);
                sqlite3_bind_text(statement, 2, media.file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 3, media.parent_file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 4, media.full_path.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 5, media.mime_type.c_str(),-1,NULL);
                sqlite3_bind_int64(statement, 6, media.size);
                sqlite3_bind_text(statement, 7, media.icon.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 8, media.file_name.c_str(), -1, NULL);
                sqlite3_bind_text(statement, 9, media.revId.c_str(), -1, NULL);
                sqlite3_bind_int64(statement, 10, media.last_mod_date);
                sqlite3_bind_int64(statement, 11, media.id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
        }
        catch (...) {
            success = false;
        }
        
        return success;
    }
    
    bool ODDbInfo::UpdateOrAddTempMediaForAccount(media_description_t& media)
    {
        media_description_t dbMedia = GetTempMediaForFileId(media.account_id, media.file_id);
        ID id = dbMedia.id;
        if (id <= 0) {
            return AddTempMediaForAccount(media);
        }
        else
        {
            media.id = id;
            if (dbMedia.revId != media.revId) {
                return UpdateTempMedia(media);
            }
            else
                return true;
        }
        return false;
    }

    void ODDbInfo::GetAllTempMedia(media_description_vector_t& mediaList)
    {
        std::string	query = "SELECT * FROM temp_media_info";
        
		int result = 0;
        
		sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
                    media_description_t media;
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
                    mediaList.push_back(media);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
        
    }

    bool ODDbInfo::RemoveTempMedia(const ID& id)
    {
        bool success = false;
        try {
            std::string query = "DELETE FROM temp_media_info WHERE id=?";
            sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_int64(statement, 1, id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
            }
            success = true;
        } catch (...) {
            success = false;
        }
        return success;
    }

    //TODO: Optimization needed here....
    void ODDbInfo::CommitTempAdditions(media_description_t& mediaDesc, media_progress_callback cb)
    {
        if (mediaDesc.id <= 0) {
            return;
        }
        
        media_description_t parentMediaDesc = GetMediaForFileId(mediaDesc.account_id, mediaDesc.parent_file_id);
        
        if (parentMediaDesc.id <= 0 && mediaDesc.parent_file_id != "/") {
            media_description_t tempParentMediaDesc = GetTempMediaForFileId(mediaDesc.account_id, mediaDesc.parent_file_id);
            CommitTempAdditions(tempParentMediaDesc, cb);
            
            if (tempParentMediaDesc.parent_id > 0)
                parentMediaDesc = tempParentMediaDesc;
        }
        
        if (mediaDesc.parent_file_id == "/") {
            mediaDesc.parent_id = -1;
        }
        else if (parentMediaDesc.id > 0)
            mediaDesc.parent_id = parentMediaDesc.id;
        else
            return;
        
        if (mediaDesc.full_path.empty()) {
            mediaDesc.full_path = parentMediaDesc.full_path + "/" + mediaDesc.file_name;
        }
        
        RemoveTempMedia(mediaDesc.id);
        AddMediaForAccount(mediaDesc);
        cb(RESULT_NoErr, mediaDesc);
    }
    
    void ODDbInfo::GetNewlyUpdatedMedia(media_progress_callback cb)
    {
        media_description_vector_t tempMediaList;
        GetAllTempMedia(tempMediaList);
        
        for (media_description_vector_t::iterator iter = tempMediaList.begin(); iter != tempMediaList.end(); iter++) {
            media_description_t mediaDesc = *iter;
            bool bExists = GetMediaForFileId(mediaDesc.account_id, mediaDesc.file_id).id > 0;
            
            if (!bExists) {
                CommitTempAdditions(mediaDesc, cb);
            }
        }
        
//        std::string query = "DELETE FROM temp_media_info";
//        RunQuery(query.c_str());
    }
    
    ID ODDbInfo::GetMedia(const int accountId, const std::string& fileId)
    {
        ID mediaId = -1;
        try {
            std::string query = "SELECT id FROM media_info WHERE account_id=? AND file_id=?";
           	sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, accountId);
                sqlite3_bind_text(statement, 2, fileId.c_str(), -1, NULL);
                int result = sqlite3_step(statement);
                if(result == SQLITE_ROW)
                    mediaId = sqlite3_column_int64(statement, 0);
                sqlite3_finalize(statement);
            }
        } catch (...) {
        }
        return mediaId;
    }

    ID ODDbInfo::GetMediaID(const std::string& fullpath)
    {
        ID mediaId = -1;
        try {
            std::string query = "SELECT id FROM media_info WHERE full_path=?";
           	sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_text(statement, 1,  fullpath.c_str(), -1, NULL);
                int result = sqlite3_step(statement);
                if(result == SQLITE_ROW)
                    mediaId = sqlite3_column_int64(statement, 0);
                sqlite3_finalize(statement);
            }
        } catch (...) {
        }
        return mediaId;
    }
    
    bool ODDbInfo::UpdateMedia(media_description_t& media)
    {
        bool success = true;
        try {
            BeginTransaction();
            std::string updateQuery;
            updateQuery += "UPDATE media_info SET parent_id = ?, file_id = ?, parent_file_id = ?, full_path = ?, mime_type = ?, size = ?, icon = ?, asset_name = ?, revision_id = ?, last_mod_date = ? ";
            updateQuery += "WHERE id = ?;";
            
           	sqlite3_stmt* statement;
            bool rc = PrepareDB(updateQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_int64(statement, 1, media.parent_id);
                sqlite3_bind_text(statement, 2, media.file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 3, media.parent_file_id.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 4, media.full_path.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 5, media.mime_type.c_str(),-1,NULL);
                sqlite3_bind_int64(statement, 6, media.size);
                sqlite3_bind_text(statement, 7, media.icon.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 8, media.file_name.c_str(),-1,NULL);
                sqlite3_bind_text(statement, 9, media.revId.c_str(), -1, NULL);
                sqlite3_bind_int64(statement, 10, media.last_mod_date);
                sqlite3_bind_int64(statement, 11, media.id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
        }
        catch (...) {
            success = false;
        }
        
        return success;
    }

    bool ODDbInfo::RemoveMedia(const ID& id)
    {
        bool success = false;
        try {
            std::string query = "DELETE FROM media_info WHERE id=?";
            sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_int64(statement, 1, id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
            }
            success = true;
        } catch (...) {
            success = false;
        }
        return success;
    }
    
    std::string ODDbInfo::GetPathForFileId(const std::string& fileId)
    {
        std::string path;
        try {
            std::string query = "SELECT full_path FROM media_info WHERE file_id=?";
          	sqlite3_stmt* statement;
            bool rc = PrepareDB(query.c_str(), &statement);
            if (rc){
                sqlite3_bind_text(statement, 1,  fileId.c_str(), -1, NULL);
                int result = sqlite3_step(statement);
                if(result == SQLITE_ROW)
                    path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
                sqlite3_finalize(statement);
            }
        } catch (...) {
        }
        return path;
    }
    
    media_description_t ODDbInfo::GetMediaForFileId(const int& accountId, const std::string& fileId)
    {
        std::string	query = "SELECT * FROM media_info WHERE account_id=? AND file_id=?";
        media_description_t media;
		int result = 0;
        
		sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int(statement, 1, accountId);
            sqlite3_bind_text(statement, 2, fileId.c_str(),-1,NULL);
            
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
        
        return media;
        
    }
    
    media_description_t ODDbInfo::GetMediaForId(const ID& mediaId)
    {
        std::string	query = "SELECT * FROM media_info WHERE id=?";
        media_description_t media;
		int result = 0;
        
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int64(statement, 1, mediaId);
            
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
        
        return media;
        
    }
    
    void ODDbInfo::GetMediaListForParent(const ID& parentId, const int& accountId, media_description_vector_t& childList)
    {
        std::string	query = "SELECT * FROM media_info WHERE parent_id=? AND account_id=? AND is_folder=1";
        media_description_t media;
		int result = 0;
        
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int64(statement, 1, parentId);
            sqlite3_bind_int(statement, 2, accountId);
            
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
                    
                    childList.push_back(media);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
    }

    void traceCallback( void* udp, const char* sql )
    {
        printf("{SQL} [%s]\n", sql);
    }
    void ODDbInfo::GetMediaByProperties(const std::string &relativePath, const std::string& searchString, int accountId, media_progress_callback callback)
    {
        //FIXME (abedi): Fix the query...
        // We might want to restrict the search for particular account or service... Will fix this after discussion.
		std::string	query = "SELECT * FROM media_info WHERE asset_name LIKE '%' || ? || '%' || ? || '%'";
        if(relativePath != "")
        {
            query = "SELECT * FROM media_info WHERE full_path LIKE '%' || ? || '%' || ? || '%'";
        }
        if (accountId != -1) {
            query += " AND account_id = ?";
        }
        
		int result = 0;
        
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_text(statement, 1, relativePath.c_str(),-1,NULL);
            sqlite3_bind_text(statement, 2, searchString.c_str(),-1,NULL);
            if (accountId != -1) {
                sqlite3_bind_int(statement, 3, accountId);
            }
            
            sqlite3_trace(mDBHandle, traceCallback, NULL);
            
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media_description_t media;
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
                    callback(RESULT_NoErr, media);
				}
				else
					break;
			}
            mStopCurrentQuery = false; // Reset the flag
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
    }

    void ODDbInfo::StopCurrentRunningQuery()
    {
        mStopCurrentQuery = true;
    }
    
    void ODDbInfo::GetMediaByMimeType(const std::string& mimeType, int accountId, media_progress_callback callback)
    {
		std::string	query = "SELECT * FROM media_info WHERE mime_type LIKE '%' || ? || '%'";
        if (accountId != -1) {
            query += " AND account_id = ?";
        }

		int result = 0;
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
			sqlite3_bind_text(statement, 1, mimeType.c_str(),-1,NULL);
            if (accountId != -1) {
                sqlite3_bind_int(statement, 2, accountId);
            }

            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media_description_t media;
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
                    callback(RESULT_NoErr, media);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
    }
    
    void ODDbInfo::GetFavoriteMedia(int accountId, media_progress_callback callback)
    {
        int desc_id = GetDescriptionIdByIdentifier(metadata_description::isFavorite, metadata_type::integer_type);
        std::string query = "SELECT * FROM media_info WHERE id IN ";
        query += "(SELECT id FROM media_metadata_info WHERE metadata_id IN ";
        query += "(SELECT id FROM metadata_integer_info WHERE description_id = ? AND value = ?)";
        query += ")";
        if (accountId != -1) {
            query += " AND account_id = ?";
        }

		int result = 0;

        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int(statement, 1, desc_id);
            sqlite3_bind_int(statement, 2, 1);
            if (accountId != -1) {
                sqlite3_bind_int(statement, 3, accountId);
            }
            
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					media_description_t media;
					media.id = sqlite3_column_int64(statement, 0);
                    media.account_id = sqlite3_column_int(statement, 1);
                    media.parent_id = sqlite3_column_int64(statement, 2);
                    media.file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
                    media.parent_file_id = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    media.full_path = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
                    media.mime_type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
                    media.is_folder = sqlite3_column_int(statement, 7);
                    media.size = sqlite3_column_int64(statement, 8);
                    media.icon = reinterpret_cast<const char*>(sqlite3_column_text(statement, 9));
                    media.file_name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 10));
                    media.revId = reinterpret_cast<const char*>(sqlite3_column_text(statement, 11));
                    media.last_mod_date = sqlite3_column_int64(statement, 12);
                  //  if(media.is_folder)
                    //    continue;// Lets not add folders as favorites. It complicates lots of stuff. Not for version 1.0 atleast.
                    callback(RESULT_NoErr, media);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
        
    }
    
    int ODDbInfo::GetMediaDownloadStatus(const ID& media_id)
    {
        int desc_id = GetDescriptionIdByIdentifier(metadata_description::downloadStatus, metadata_type::integer_type);
        int value = -1;
        std::string	query = "SELECT value ";
        query += "FROM metadata_integer_info INNER JOIN media_metadata_info ";
        query += "ON metadata_integer_info.id = media_metadata_info.metadata_id AND media_metadata_info.id = ? ";
        query += "AND metadata_integer_info.description_id = ?";
        
		sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int64(statement, 1, media_id);
            sqlite3_bind_int(statement, 2, desc_id);
            while(true){
				int result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					value = sqlite3_column_int(statement, 0);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
        return value;
    }
    
    std::string ODDbInfo::GetMediaDownloadPath(const ID& media_id) 
    {
        int desc_id = GetDescriptionIdByIdentifier(metadata_description::downloadPath, metadata_type::string_type);
        std::string value;
        std::string	query = "SELECT value ";
        query += "FROM metadata_string_info INNER JOIN media_metadata_info ";
        query += "ON metadata_string_info.id = media_metadata_info.metadata_id AND media_metadata_info.id = ? ";
        query += "AND metadata_string_info.description_id = ?";
        
		sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int64(statement, 1, media_id);
            sqlite3_bind_int(statement, 2, desc_id);
            while(true){
				int result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
                    value = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        return value;
    }
    
    std::string ODDbInfo::GetMediaDownloadRevision(const ID& media_id)
    {
        int desc_id = GetDescriptionIdByIdentifier(metadata_description::downloadRevision, metadata_type::string_type);
        std::string value;
        std::string	query = "SELECT value ";
        query += "FROM metadata_string_info INNER JOIN media_metadata_info ";
        query += "ON metadata_string_info.id = media_metadata_info.metadata_id AND media_metadata_info.id = ? ";
        query += "AND metadata_string_info.description_id = ?";
        
		sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int64(statement, 1, media_id);
            sqlite3_bind_int(statement, 2, desc_id);
            while(true){
				int result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
                    value = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        return value;
    }
    
    void ODDbInfo::UpdateMediaDownloadDetails(const ID& media_id, const std::string& downloadPath, const std::string& downloadRevision)
    {
        int download_desc = GetDescriptionIdByIdentifier(metadata_description::downloadPath, metadata_type::string_type);
        int revision_desc = GetDescriptionIdByIdentifier(metadata_description::downloadRevision, metadata_type::string_type);
        try {
            BeginTransaction();
            std::string updateQuery;
            updateQuery += "UPDATE metadata_string_info SET value = ? WHERE ";
            updateQuery += "description_id = ? AND id IN (";
            updateQuery += "SELECT metadata_id FROM media_metadata_info WHERE id = ?)";
            
            sqlite3_stmt* statement1;
            bool rc = PrepareDB(updateQuery.c_str(), &statement1);
            if (rc){
                sqlite3_bind_text(statement1, 1, downloadPath.c_str(), -1, NULL);
                sqlite3_bind_int(statement1, 2, download_desc);
                sqlite3_bind_int64(statement1, 3, media_id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement1);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement1);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
            updateQuery = "UPDATE metadata_string_info SET value = ? WHERE ";
            updateQuery += "description_id = ? AND id IN (";
            updateQuery += "SELECT metadata_id FROM media_metadata_info WHERE id = ?)";
            
            sqlite3_stmt* statement2;
            rc = PrepareDB(updateQuery.c_str(), &statement2);
            if (rc){
                sqlite3_bind_text(statement2, 1, downloadRevision.c_str(), -1, NULL);
                sqlite3_bind_int(statement2, 2, revision_desc);
                sqlite3_bind_int64(statement2, 3, media_id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement2);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement2);
            }
            error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
            EndTransaction();
        } catch (...) {
            
        }
        
    }
    
    void ODDbInfo::AddMediaDownloadDetails(const ID& media_id, const std::string& downloadPath, const std::string& downloadRevision)
    {
        int metadata_download_id = 0;
        int metadata_revision_id = 0;
        try {
            int download_desc = GetDescriptionIdByIdentifier(metadata_description::downloadPath, metadata_type::string_type);
            int revision_desc = GetDescriptionIdByIdentifier(metadata_description::downloadRevision, metadata_type::string_type);
            
            BeginTransaction();
            
            std::string insertQuery;
            insertQuery += "INSERT INTO metadata_string_info (\"id\", \"description_id\", \"value\") ";
            insertQuery += "VALUES (?, ?, ?), (?, ?, ?);";
            
            sqlite3_stmt* statement1;
            bool rc = PrepareDB(insertQuery.c_str(), &statement1);
            if (rc){
                metadata_download_id = GetNewRecordAutoId();
                sqlite3_bind_int(statement1, 1, metadata_download_id);
                sqlite3_bind_int(statement1, 2, download_desc);
                sqlite3_bind_text(statement1, 3, downloadPath.c_str(),-1,NULL);
                metadata_revision_id = GetNewRecordAutoId();
                sqlite3_bind_int(statement1, 4, metadata_revision_id);
                sqlite3_bind_int(statement1, 5, revision_desc);
                sqlite3_bind_text(statement1, 6, downloadRevision.c_str(),-1,NULL);
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement1);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement1);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
            insertQuery = "INSERT INTO media_metadata_info (\"id\", \"metadata_id\") ";
            insertQuery += "VALUES (?, ?), (?, ?);";
            
            sqlite3_stmt* statement2;
            rc = PrepareDB(insertQuery.c_str(), &statement2);
            if (rc){
                sqlite3_bind_int64(statement2, 1, media_id);
                sqlite3_bind_int(statement2, 2, metadata_download_id);
                
                sqlite3_bind_int64(statement2, 3, media_id);
                sqlite3_bind_int(statement2, 4, metadata_revision_id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement2);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement2);
            }
            
            EndTransaction();
            
        } catch (...) {
        }
    }

    void ODDbInfo::AddMediaMetadata(const ID& media_id, const std::string& identifier, const std::string& value)
    {
        int metadata_id = AddMediaStringMetadata(identifier, value);
        AddMetadata(media_id, metadata_id);
    }
    
    void ODDbInfo::AddMediaMetadata(const ID& media_id, const std::string& identifier, const int& value)
    {
        int metadata_id = AddMediaIntegerMetadata(identifier, value);
        AddMetadata(media_id, metadata_id);
    }
    
    void ODDbInfo::UpdateMediaMetadata(const ID& media_id, const std::string& identifier, const std::string& value)
    {
        int desc_id = GetDescriptionIdByIdentifier(identifier, metadata_type::string_type);
        try {
            BeginTransaction();
            std::string updateQuery;
            updateQuery += "UPDATE metadata_string_info SET value = ? WHERE ";
            updateQuery += "description_id = ? AND id IN (";
            updateQuery += "SELECT metadata_id FROM media_metadata_info WHERE id = ?)";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(updateQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_text(statement, 1, value.c_str(), -1, NULL);
                sqlite3_bind_int(statement, 2, desc_id);
                sqlite3_bind_int64(statement, 3, media_id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
        } catch (...) {
            
        }
    }
    
    void ODDbInfo::UpdateMediaMetadata(const ID& media_id, const std::string& identifier, const int& value)
    {
        int desc_id = GetDescriptionIdByIdentifier(identifier, metadata_type::integer_type);
        try {
            BeginTransaction();
            std::string updateQuery;
            updateQuery += "UPDATE metadata_integer_info SET value = ? WHERE ";
            updateQuery += "description_id = ? AND id IN (";
            updateQuery += "SELECT metadata_id FROM media_metadata_info WHERE id = ?)";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(updateQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_int(statement, 1, value);
                sqlite3_bind_int(statement, 2, desc_id);
                sqlite3_bind_int64(statement, 3, media_id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
        } catch (...) {
            
        }
    }
    
    void ODDbInfo::AddMetadata(const ID& media_id, const int& metadata_id)
    {
        try {
            BeginTransaction();
            std::string insertQuery;
            insertQuery += "INSERT INTO media_metadata_info (\"id\", \"metadata_id\") ";
            insertQuery += "VALUES (?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                sqlite3_bind_int64(statement, 1, media_id);
                sqlite3_bind_int(statement, 2, metadata_id);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
            
        } catch (...) {
        }
    }
    
    int ODDbInfo::GetDescriptionIdByIdentifier(const std::string& identifier, const std::string& data_type)
    {
        std::string	query = "SELECT id FROM metadata_description_info WHERE identifier = ? AND data_type = ?";
		int desc_id = -1;
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
			sqlite3_bind_text(statement, 1, identifier.c_str(),-1,NULL);
			sqlite3_bind_text(statement, 2, data_type.c_str(),-1,NULL);
            while(true){
				int result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
                    desc_id = sqlite3_column_int(statement, 0);
                }
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
        if (desc_id == -1) {
            desc_id = AddMetadataDescriptions(identifier, data_type);
        }
        
        return desc_id;
    }
    
    void ODDbInfo::SetMediaAsFavorite(const ID &media_id, bool set)
    {
        int cur_value = IsMediaFavorite(media_id);
        if ( cur_value == -1) { // does not exist
            AddMediaMetadata(media_id, metadata_description::isFavorite, set);
        }
        else if ( cur_value != set)
        {
            UpdateMediaMetadata(media_id, metadata_description::isFavorite, set);
        }
    }
    
    int ODDbInfo::IsMediaFavorite(const ID& media_id)
    {
        int desc_id = GetDescriptionIdByIdentifier(metadata_description::isFavorite, metadata_type::integer_type);
        int value = -1; //value does not exist
        std::string	query = "SELECT value FROM metadata_integer_info ";
        query += "INNER JOIN media_metadata_info ";
        query += "ON metadata_integer_info.id = media_metadata_info.metadata_id AND media_metadata_info.id = ? ";
        query += "AND metadata_integer_info.description_id = ?";
        
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            sqlite3_bind_int64(statement, 1, media_id);
            sqlite3_bind_int(statement, 2, desc_id);
            while(true){
				int result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
					value = sqlite3_column_int(statement, 0);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
        return value;
    }

    void ODDbInfo::AddRecentItem(const ID& item)
    {
        std::vector<ID> recentList;
        GetRecents(recentList);
        
        if (std::find(recentList.begin(), recentList.end(), item) != recentList.end()) {
            try {
                BeginTransaction();
                
                std::string deleteQuery;
                deleteQuery += "DELETE FROM recent_info WHERE media_id = ?";
                
                sqlite3_stmt* statement;
                bool rc = PrepareDB(deleteQuery.c_str(), &statement);
                if (rc){
                    sqlite3_bind_int(statement, 1, item);
                    int result = 0;
                    while (true)
                    {
                        result = sqlite3_step(statement);
                        if (result != SQLITE_ROW)
                            break;
                    }
                    sqlite3_finalize(statement);
                }
                std::string error = sqlite3_errmsg(mDBHandle);
                NSLog(@"%s", error.c_str());
                
                EndTransaction();
                
            } catch (...) {
            }
        }
        
        try {
            BeginTransaction();
            
            std::string insertQuery;
            insertQuery += "INSERT INTO recent_info (\"id\", \"media_id\") ";
            insertQuery += "VALUES (?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                ID id = GetNewRecordAutoId();
                sqlite3_bind_int(statement, 1, id);
                sqlite3_bind_int(statement, 2, item);
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            
            EndTransaction();
            
        } catch (...) {
        }
    }
    
    void ODDbInfo::GetRecents(std::vector<ID>& recentList)
    {
        std::string query = "SELECT media_id FROM recent_info ORDER by id DESC";
        
		int result = 0;
        
        sqlite3_stmt* statement;
        bool rc = PrepareDB(query.c_str(), &statement);
        if (rc){
            while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
                    ID id = sqlite3_column_int64(statement, 0);
                    recentList.push_back(id);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
    }

    int ODDbInfo::AddMediaStringMetadata(const std::string& identifier, const std::string& value)
    {
        int metadata_id = 0;
        try {
            int desc_id = GetDescriptionIdByIdentifier(identifier, metadata_type::string_type);
            
            BeginTransaction();
            
            std::string insertQuery;
            insertQuery += "INSERT INTO metadata_string_info (\"id\", \"description_id\", \"value\") ";
            insertQuery += "VALUES (?, ?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                metadata_id = GetNewRecordAutoId();
                sqlite3_bind_int(statement, 1, metadata_id);
                sqlite3_bind_int(statement, 2, desc_id);
                sqlite3_bind_text(statement, 3, value.c_str(),-1,NULL);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
            
        } catch (...) {
        }
        return metadata_id;
    }
    
    int ODDbInfo::AddMediaIntegerMetadata(const std::string& identifier, const int& value)
    {
        int metadata_id = 0;
        try {
            int desc_id = GetDescriptionIdByIdentifier(identifier, metadata_type::integer_type);
            
            BeginTransaction();
            
            std::string insertQuery;
            insertQuery += "INSERT INTO metadata_integer_info (\"id\", \"description_id\", \"value\") ";
            insertQuery += "VALUES (?, ?, ?);";
            
            sqlite3_stmt* statement;
            bool rc = PrepareDB(insertQuery.c_str(), &statement);
            if (rc){
                metadata_id = GetNewRecordAutoId();
                sqlite3_bind_int(statement, 1, metadata_id);
                sqlite3_bind_int(statement, 2, desc_id);
                sqlite3_bind_int(statement, 3, value);
                
                int result = 0;
                while (true)
                {
                    result = sqlite3_step(statement);
                    if (result != SQLITE_ROW)
                        break;
                }
                sqlite3_finalize(statement);
            }
            std::string error = sqlite3_errmsg(mDBHandle);
            NSLog(@"%s", error.c_str());
            EndTransaction();
            
        } catch (...) {
        }
        return metadata_id;
    }
    bool ODDbInfo::PrepareDB(const char *query, sqlite3_stmt** statement)
    {
        int numberOfRetries = 0;
        BOOL retry          = NO;
        int rc = 0;
        bool logErrors = false; // Switch On for debugging.
        do {
            retry   = NO;
            rc    = sqlite3_prepare(mDBHandle, query, -1, statement, 0);
            
            if (SQLITE_BUSY == rc) {
                retry = YES;
                usleep(20);
                
                if (busyRetryTimeout && (numberOfRetries++ > busyRetryTimeout)) {
                    sqlite3_finalize(*statement);
                    return false;
                }
            }
            else if (SQLITE_OK != rc) {
                
                
                if (logErrors) {
                    NSLog(@"DB Error on DB operation \"%s\"",query);
                    NSCAssert(false, @"DB Error: ");
                    
                }
                
                sqlite3_finalize(*statement);
                return false;
            }
        }
        while (retry);

        return true;
    }
    
	bool ODDbInfo::RunQuery(const char* query)
	{
        sqlite3_stmt* statement;
     	bool rc = PrepareDB(query, &statement);
		if (rc == true)
        {
            int result = 0;
            while (true)
            {
                result = sqlite3_step(statement);
                if (result == SQLITE_ROW)
                {
                }
                else
                    break;
            }
            sqlite3_finalize(statement);
       
        }
      

		std::string error = sqlite3_errmsg(mDBHandle);
		NSLog(@"%s", error.c_str());
        return rc;
	}
}