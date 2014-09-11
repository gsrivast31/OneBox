//
//  Types.h
//  DataStorage
//
//  Created by OneDrive on 12/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef DataStorage_Types_h
#define DataStorage_Types_h

#include "Types.h"

#include <vector>
#include <string>

namespace  {

    using namespace OneDrive;
    
  //  typedef int ID;
	typedef ID mediaId;
	typedef ID accountId;
	typedef ID parentId;
	typedef ID serviceId;
    typedef bool Status;
    typedef std::string String;
	typedef std::vector<ID> ODIdList;
	typedef ODIdList ODMediaIdList;
	typedef ODIdList ODAccountIdList;
	typedef std::vector<String> ODStringList;

	enum 
	{
		eModelMediaImage,
		eModelMediaVideo,
		eModelMediaDocument,
		eModelMediaPDF,
		eModelMediaContact,
		eModelMediaFolder,
		eModelMediaUnknown
	}MediaType;
}

#endif
