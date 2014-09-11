//
//  ODThumbInfo.cpp
//  OneDrive
//
//  Created by Gaurav Srivastava on 12/29/12.
//  Copyright (c) 2012 Labs. All rights reserved.
//

#include "ODThumbInfo.h"
#include <functional>
#import <Foundation/Foundation.h>

namespace NewGroup
{
	ODThumbInfo::ODThumbInfo()
    :mDBHandle(NULL)
	{
	}
    
	void ODThumbInfo::InitDB(const std::string& databaseDir )
	{
		mDatabasePath = databaseDir + "/thumb.cache";
		int err_code = sqlite3_open_v2(mDatabasePath.c_str(), &mDBHandle,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX,NULL);
		if (err_code != SQLITE_OK)
		{
			std::string err = sqlite3_errmsg(mDBHandle);
			sqlite3_close(mDBHandle);
            return;
		}
        
       CreateTables();
	}
    
	void ODThumbInfo::CreateTables()
	{
        if(!mDBHandle)
            return;
        try {
            std::string query;
            query += "CREATE TABLE IF NOT EXISTS thumb_info (";
            query += "thumb_id INTEGER PRIMARY KEY AUTOINCREMENT, ";
            query += "media_id INTEGER NOT NULL, ";
            query += "width INTEGER NOT NULL, ";
            query += "height INTEGER NOT NULL, ";
            query += "thumbnailPath VARCHAR(4096)";
            query += ");";
            
            RunQuery(query.c_str());
            
        } catch (...) {
            
        }
	}
	
	void ODThumbInfo::UnInit()
	{
		sqlite3_close(mDBHandle);
	}
    
    bool ODThumbInfo::InsertThumbForMedia(thumb_description_t& thumb)
    {
        try {
            std::string query = "BEGIN TRANSACTION";
            RunQuery(query.c_str());
            
            std::string insertQuery;
            insertQuery += "INSERT INTO thumb_info (\"media_id\", \"width\", \"height\", \"thumbnailPath\") ";
            insertQuery += "VALUES (?, ?, ?, ?);";
            
            sqlite3_stmt* statement;
            if (sqlite3_prepare_v2(mDBHandle, insertQuery.c_str(), -1, &statement, 0) == SQLITE_OK){
                sqlite3_bind_int64(statement, 1, thumb.media_id);
                
                sqlite3_bind_int(statement, 2, thumb.width);
                sqlite3_bind_int(statement, 3, thumb.height);
                
                sqlite3_bind_text(statement, 4, thumb.thumbnailPath.c_str(), -1, NULL);
                
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
            
        }
        catch (...) {
            return false;
        }
        
        thumb.thumb_id = GetThumbId(thumb.media_id, thumb.width, thumb.height);
        
        return true;
    }
    
    bool ODThumbInfo::UpdateThumbForMedia(thumb_description_t& thumb)
    {
        bool success = true;
        try {
            std::string query = "BEGIN TRANSACTION";
            RunQuery(query.c_str());
            
            std::string updateQuery;
            updateQuery += "UPDATE thumb_info SET thumbnailPath = ?";
            updateQuery += "WHERE thumb_id = ?;";
            
            sqlite3_stmt* statement;
            if (sqlite3_prepare_v2(mDBHandle, updateQuery.c_str(), -1, &statement, 0) == SQLITE_OK){
                sqlite3_bind_text(statement, 1, thumb.thumbnailPath.c_str(),-1,NULL);
                sqlite3_bind_int64(statement, 2, thumb.thumb_id);
                
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
        }
        catch (...) {
            success = false;
        }
        
        return success;
        
    }

    void ODThumbInfo::GetThumbForMedia(const ID& mediaId, int width, int height,
                                                      thumb_progress_callback cb)
    {
		std::string	query = "SELECT * FROM thumb_info WHERE media_id=? AND (width=? OR height=? )";
		sqlite3_stmt* statement;
		int result = 0;
        
       if (sqlite3_prepare_v2(mDBHandle, query.c_str(), -1, &statement, 0) == SQLITE_OK){
            sqlite3_bind_int64(statement, 1, mediaId);
            sqlite3_bind_int(statement, 2, width);
            sqlite3_bind_int(statement, 3, height);
            
			while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
                    thumb_description_t thumb;
                    
					thumb.thumb_id = sqlite3_column_int64(statement, 0);
                    thumb.media_id = sqlite3_column_int64(statement, 1);

                    thumb.width = sqlite3_column_int(statement, 2);
                    thumb.height = sqlite3_column_int(statement, 3);
                    
                    thumb.thumbnailPath = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));
                    
                    cb(RESULT_NoErr, thumb);
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
		result = sqlite3_errcode(mDBHandle);
		std::string error = sqlite3_errmsg(mDBHandle);
    }
    
    bool ODThumbInfo::IsThumbnailEmpty(const ID& mediaId, int width, int height)
    {
    	std::string	query = "SELECT thumbnailPath FROM thumb_info WHERE media_id=? AND (width=? OR height=? )";
		sqlite3_stmt* statement;
		int result = 0;
        
        if (sqlite3_prepare_v2(mDBHandle, query.c_str(), -1, &statement, 0) == SQLITE_OK){
            sqlite3_bind_int64(statement, 1, mediaId);
            sqlite3_bind_int(statement, 2, width);
            sqlite3_bind_int(statement, 3, height);
            
			while(true){
				result = sqlite3_step(statement);
				if (result == SQLITE_ROW){
                    std::string thumbPath = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
                    return thumbPath.empty();
				}
				else
					break;
			}
			sqlite3_finalize(statement);
		}
        
        return true;
    }
    
    ID ODThumbInfo::GetThumbId(const ID& mediaId, int width, int height)
    {
        ID thumb_id = 0;
        try {
            std::string	query = "SELECT thumb_id FROM thumb_info WHERE media_id=? AND (width=? OR height=? )";
            sqlite3_stmt* statement;
            
            if (sqlite3_prepare_v2(mDBHandle, query.c_str(), -1, &statement, 0) == SQLITE_OK){
                sqlite3_bind_int64(statement, 1, mediaId);
                sqlite3_bind_int(statement, 2, width);
                sqlite3_bind_int(statement, 3, height);
                
                int result = sqlite3_step(statement);
                if(result == SQLITE_ROW)
                    thumb_id = sqlite3_column_int64(statement, 0);
                sqlite3_finalize(statement);
            }
        } catch (...) {
        }
        return thumb_id;
    }
    
    bool ODThumbInfo::UpdateOrAddThumbnailForMedia(thumb_description_t& thumb)
    {
        ID thumbId = GetThumbId(thumb.media_id, thumb.width, thumb.height);
        if (thumbId == 0) {
            return InsertThumbForMedia(thumb);
        }
        else
        {
            thumb.thumb_id = thumbId;
            return UpdateThumbForMedia(thumb);
        }
    }

    bool ODThumbInfo::RemoveThumb(const ID& mediaId)
    {
        bool success = false;
        try {
            std::string query = "DELETE FROM thumb_info WHERE media_id=?";
            sqlite3_stmt* statement;
            if (sqlite3_prepare_v2(mDBHandle, query.c_str(), -1, &statement, 0) == SQLITE_OK){
                sqlite3_bind_int64(statement, 1, mediaId);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
            }
            success = true;
        } catch (...) {
            success = false;
        }
        return success;
    }
    
	void ODThumbInfo::RunQuery(const char* query)
	{
		sqlite3_stmt* statement;
		
		if (sqlite3_prepare_v2(mDBHandle, query, -1, &statement, 0) == SQLITE_OK)
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
	}
}