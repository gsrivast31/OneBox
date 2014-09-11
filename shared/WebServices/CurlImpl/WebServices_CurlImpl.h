#pragma once
#include "WebServicesInterface.h"
#include <functional>
#include "ThreadPool.h"
#include "curl/curl.h"

namespace NewGroup
{
	namespace Core
	{
        
		class WebServices_CurlImpl: public WebServicesInterface
		{
					
			void SetURLParameter(UTF8String &strParams, const char *param, const char *value);
			void SetHTTPHeader(const char *param, const char *value);	
			void SendRequest(const char *method, bool isGetRequest, const UString& uri, Utils::BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData);
			static size_t responseCallBackProc_s( char *ptr, size_t size, size_t nmemb, void *userdata);
            static int progressCallBackProc_s(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
            static size_t headerCallBackProc_s( char *ptr, size_t size, size_t nmemb, void *userdata);
            
			size_t responseCallBackProc(const char *ptr, size_t size);
            int progressCallBackProc(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
            void headerCallBackProc( char *ptr, size_t size);
            
			int ExecuteRequest(const UserDataType_t&);
			CURL *m_CurlHandle;
			static ThreadPool mTasksPool;
			CallBackProc_t mCallBackProc;
			curl_slist *mHTTPHeader;
			char mErrorBuffer[CURL_ERROR_SIZE];
            std::vector<unsigned char> mResponseData;
            ProgressProc_t mProgressCallback;
            DataCallBackProc_t mDataCallbackProc;
            UserDataType_t mUserData;
            HeaderListType_t mHeadersList;
            bool mStoreHeaders;
            
			WebServices_CurlImpl(CallBackProc_t callBackFunction, DataCallBackProc_t dataCallback, ProgressProc_t progressCallback, bool notifyHeaders);
            
			~WebServices_CurlImpl();	

            //Interface Implementation
			virtual void SetRequestHeader(const UString& key, const UString& value);
            UString EncodeURL(const UString& uri, const ParametersType_t& params);
			virtual void SendGetRequest(const UString& uri, const ParametersType_t& params, const UserDataType_t& userData);
            
			virtual void SendPostRequest(const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData);
			virtual void SendPutRequest(const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData);
			virtual void SendRequestByMethod(const char* method, const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData);

            void SendPostRequest(const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData);
			void SendPutRequest(const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData);
			void SendRequestByMethod(const char* method, const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData);
            
			virtual void DownloadFile();
			virtual void UploadFile();
            
            void EncodeURL(UTF8String &encoded, const UString& uri, const ParametersType_t& params);
            
            friend WebServicesInterface::WSEngineSharedPtr WebServicesInterface::CreateEngineInstance(CallBackProc_t callBackFunction, DataCallBackProc_t dataCallback, ProgressProc_t progressCallback, bool);
            
            
		};
	}
}