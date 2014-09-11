#pragma once
#include "UString.h"
#include "Exception.h"
#include "ODAssert.h"
#include <map>
#include <vector>
#include <functional>
#include "CommonUtils.h"
namespace NewGroup
{
	namespace Core
	{		 		
		enum WebServiceEngineType
		{
			WebServicesUsingCurl
		};
        
        using namespace Utils;
		class WebServicesInterface: public std::enable_shared_from_this<WebServicesInterface>
		{
        protected:
            TaskPriority mTaskPriority;
		public:
			typedef std::map<UString, UString> UserDataType_t;
			typedef std::map<UString, UString> ParametersType_t;
            typedef std::vector<UString> HeaderListType_t;
            typedef std::shared_ptr<WebServicesInterface> WSEngineSharedPtr;
            typedef std::function<void (int, const UserDataType_t&, const UString& response, const HeaderListType_t& headers)> CallBackProc_t;
            typedef std::function<void (int, const UserDataType_t&, const unsigned char* response, size_t size, const HeaderListType_t& headers)> DataCallBackProc_t;
            typedef std::function<int (double total, double completed, const UserDataType_t&)> ProgressProc_t;
            
            static WSEngineSharedPtr CreateEngineInstance(CallBackProc_t callBackFunction, DataCallBackProc_t dataCallback = DataCallBackProc_t(), ProgressProc_t progressCallback = ProgressProc_t(), bool notifyHeaders = false);
            
            virtual void SetRequestHeader(const UString& key, const UString& value) = 0;
            virtual UString EncodeURL(const UString& uri, const ParametersType_t& params) = 0;
			virtual void SendGetRequest(const UString& uri, const ParametersType_t& params, const UserDataType_t& userData) = 0;
			virtual void SendPostRequest(const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData) = 0;
			virtual void SendPutRequest(const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData) = 0;
			virtual void SendRequestByMethod(const char* method, const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData) = 0;

            virtual void SendPostRequest(const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData) = 0;
			virtual void SendPutRequest(const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData) = 0;
			virtual void SendRequestByMethod(const char* method, const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData) = 0;
            
			virtual void DownloadFile() = 0;
			virtual void UploadFile() = 0;
            
            virtual ~WebServicesInterface()
            {
            }
            
            virtual void SetPriority(TaskPriority priority)
            {
                mTaskPriority = priority;
            }
		};
	}
}
