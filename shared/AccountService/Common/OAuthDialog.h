//
//  OAuthDialog.h
//  AccountServices
//
//  Created by Ruchin Kansal on 12/12/12.
//  Copyright (c) 2012 Ruchin Kansal. All rights reserved.
//

#ifndef __AccountServices__OAuthDialog__
#define __AccountServices__OAuthDialog__
#include "UString.h"
#include <iostream>
#include <memory>
#include <functional>

namespace NewGroup
{
    namespace Core
    {
        class OAuthDialog:public std::enable_shared_from_this<OAuthDialog>
        {
            
        public:
            
            typedef std::shared_ptr<OAuthDialog> OAuthDialogSharedPtr;
            typedef std::function<void (bool failed, const UString& accessToken)> AuthCallBackProc_t;
            
            OAuthDialog(PlatformView parentView, const UString& url, const UString &callBackUrl, const UString &token, AuthCallBackProc_t callback);
            ~OAuthDialog();
            
            UString GetAuthURL() const{
                return mAuthURL;
            }
            
            UString GetCallbackURL() const{
                return mCallbackURL;
            }
            
            AuthCallBackProc_t GetAuthCallbackProc() const{
                return mAuthCallbackProc;
            }
            UString GetToken() const{
                return mToken;
            }
            PlatformView GetParentView() const{
                return mParentView;
            }
            
            void SetDialogView(id view){
                mAuthDialogView = view;
            }
            
            static void ClearCookieForUrl(const UString& url);
        private:
            UString mAuthURL;
            UString mCallbackURL;
            AuthCallBackProc_t mAuthCallbackProc;
            UString mToken;
            PlatformView mParentView;
            id mAuthDialogView;
        };
    }
}

#endif /* defined(__AccountServices__OAuthDialog__) */
