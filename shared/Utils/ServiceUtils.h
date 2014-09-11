//
//  ServiceUtils.h
//  OneDrive
//
//  Created by Ajay on 12/18/12.
//  Copyright (c) 2012 Labs. All rights reserved.
//

#ifndef OneDrive_ServiceUtils_h
#define OneDrive_ServiceUtils_h

#import "AccountServiceInterface.h"

namespace ServiceUtils {
    UString GetServiceIcon(int serviceId)
    {
        if (serviceId == NewGroup::Core::kSPDropbox)
            return UString("Dropbox.png");
        else if(serviceId == NewGroup::Core::kSPGDrive)
            return UString("Google-Drive-icon.png");
        else if (serviceId == NewGroup::Core::kSPSkyDrive)
            return UString("SkyDrive.png");
        return "";
    }
}

#endif
