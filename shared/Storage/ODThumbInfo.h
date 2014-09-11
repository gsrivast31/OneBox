//
//  ODThumbInfo.h
//  OneDrive
//
//  Created by Gaurav Srivastava on 12/29/12.
//  Copyright (c) 2012 Labs. All rights reserved.
//

#ifndef __OneDrive__ODThumbInfo__
#define __OneDrive__ODThumbInfo__

#include <iostream>

#include <sqlite3.h>
#include "Types.h"
#include <string>
#include <vector>
#include <functional>

namespace NewGroup
{
    using namespace NewGroup::Core;
    
	struct thumb_description_t
	{
		thumb_description_t()
        : thumb_id(0)
		{
		}
		ID thumb_id;
        ID media_id;
        int width;
        int height;
        std::string thumbnailPath;
	};
    
 	typedef std::vector<thumb_description_t> thumb_description_vector_t;
    
	typedef std::function<void (RESULT, thumb_description_t)> thumb_progress_callback;
    
    class ODThumbInfo
	{
	public:
		ODThumbInfo();
        ~ODThumbInfo() {};
        
		void InitDB(const std::string&);
		void UnInit();
		void CreateTables();
        
        bool UpdateOrAddThumbnailForMedia(thumb_description_t&);
        bool InsertThumbForMedia(thumb_description_t&);
        bool UpdateThumbForMedia(thumb_description_t&);
        void GetThumbForMedia(const ID&, int, int, thumb_progress_callback);
        bool IsThumbnailEmpty(const ID& mediaId, int width, int height);
        bool RemoveThumb(const ID&);
        ID GetThumbId(const ID&, int, int);
        
 	private:
        void RunQuery(const char* query);
        
        std::string mDatabasePath;
		sqlite3* mDBHandle;
	};
    
} //namespace NewGroup

#endif /* defined(__OneDrive__ODThumbInfo__) */
