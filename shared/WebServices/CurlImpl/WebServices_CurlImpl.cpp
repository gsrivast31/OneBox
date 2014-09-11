#include "WebServices_CurlImpl.h"
#include "ODLogger.h"

const int kMaxSimultaneousRequests = 5;
using namespace NewGroup::Core;

ThreadPool WebServices_CurlImpl::mTasksPool(kMaxSimultaneousRequests);

WebServicesInterface::WSEngineSharedPtr WebServicesInterface::CreateEngineInstance(CallBackProc_t callBackFunction, DataCallBackProc_t dataCallback, ProgressProc_t progressCallback, bool notifyHeaders)
{
    WebServicesInterface *pInterface = new WebServices_CurlImpl(callBackFunction, dataCallback, progressCallback, notifyHeaders);
    pInterface->SetPriority(kTaskPriority_Normal);
    return WebServicesInterface::WSEngineSharedPtr(pInterface);
}

WebServices_CurlImpl::WebServices_CurlImpl(CallBackProc_t callBackFunction, DataCallBackProc_t dataCallback, ProgressProc_t progressCallback, bool notifyHeaders):mCallBackProc(callBackFunction), mDataCallbackProc(dataCallback), mProgressCallback(progressCallback),mStoreHeaders(notifyHeaders)
{
	m_CurlHandle = curl_easy_init();
	mHTTPHeader = NULL;
	memset(mErrorBuffer, 0, sizeof(mErrorBuffer));
}

WebServices_CurlImpl::~WebServices_CurlImpl()
{

}

void WebServices_CurlImpl::SetURLParameter(UTF8String &strParams, const char *param, const char *value)
{
	if(!strParams.empty())
		strParams.append("&");

	strParams.append(param);
	strParams.append("=");

	auto escapedVal = curl_easy_escape(m_CurlHandle, value, 0);
	strParams.append(escapedVal);
	curl_free(escapedVal);
}

UString WebServices_CurlImpl::EncodeURL(const UString& uri, const ParametersType_t& params)
{
    UTF8String encoded;
    EncodeURL(encoded, uri, params);
    
    return UString::fromUtf8(encoded.c_str());
}

void WebServices_CurlImpl::EncodeURL(UTF8String &encoded, const UString& uri, const ParametersType_t& params)
{
    UTF8String strParams, strURL;
    encoded = uri.getUTF8String();
	if(params.size())
	{
		for(auto paramPair:params)
		{
			SetURLParameter(strParams, paramPair.first.getUTF8String().c_str(), paramPair.second.getUTF8String().c_str());
		}
        if(uri.size())
            encoded += "?";

		encoded += strParams;
	}
}

void WebServices_CurlImpl::SetRequestHeader(const UString& key, const UString& value)
{
	//UString keyValPair = key + L":" + value + L";";
    UString keyValPair = key + L":" + value;
    mHTTPHeader = curl_slist_append(mHTTPHeader, keyValPair.getUTF8String().c_str());
}

void WebServices_CurlImpl::SendGetRequest(const UString& uri, const ParametersType_t& params, const UserDataType_t& userData)
{
    Utils::Log(UString("Starting GET request: ") + uri);
    curl_easy_setopt(m_CurlHandle, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(m_CurlHandle, CURLOPT_FOLLOWLOCATION, 1L);
	SendRequest("GET", true, uri, BinaryData_t(), params, userData);
}

void WebServices_CurlImpl::SendPostRequest(const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData)
{
    auto utf8Str = requestBody.getUTF8String();
    SendPostRequest(uri, BinaryData_t::MakeCopy((const unsigned char*)utf8Str.data(), utf8Str.size()), params, userData);
}
void WebServices_CurlImpl::SendPutRequest(const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData)
{
    auto utf8Str = requestBody.getUTF8String();
    SendPutRequest(uri, BinaryData_t::MakeCopy((const unsigned char*)utf8Str.data(), utf8Str.size()), params, userData);
}
void WebServices_CurlImpl::SendRequestByMethod(const char* method, const UString& uri, const UString& requestBody, const ParametersType_t& params, const UserDataType_t& userData)
{
    auto utf8Str = requestBody.getUTF8String();
    SendRequestByMethod(method, uri, BinaryData_t::MakeCopy((const unsigned char*)utf8Str.data(), utf8Str.size()), params, userData);
}

void WebServices_CurlImpl::SendPostRequest(const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData)
{
    Utils::Log(UString("Starting POST request: ") + uri);
	curl_easy_setopt(m_CurlHandle, CURLOPT_POST, 1L);
    
	SendRequest("POST", false, uri, requestBody, params, userData);
}

void WebServices_CurlImpl::SendPutRequest(const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData)
{
    SendRequestByMethod("PUT", uri, requestBody, params, userData);
}

void WebServices_CurlImpl::SendRequestByMethod(const char* method, const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData)
{
	curl_easy_setopt(m_CurlHandle, CURLOPT_CUSTOMREQUEST, method);
	SendRequest(method, false, uri, requestBody, params, userData);
}

void WebServices_CurlImpl::DownloadFile()
{
}

void WebServices_CurlImpl::UploadFile()
{
}

void WebServices_CurlImpl::SendRequest(const char *method, bool isGetRequest, const UString& uri, BinaryData_t requestBody, const ParametersType_t& params, const UserDataType_t& userData)
{
    Utils::Log(UString("Starting ") + UString(method) + UString(" request: ") + uri);
    
	UTF8String strURL;
	EncodeURL(strURL, uri, params);

	curl_easy_setopt(m_CurlHandle, CURLOPT_URL, strURL.c_str());

    if(!isGetRequest){
        if(requestBody.empty())
        {
            curl_easy_setopt(m_CurlHandle, CURLOPT_POSTFIELDS, NULL);
            curl_easy_setopt(m_CurlHandle, CURLOPT_POSTFIELDSIZE, 0);
        }
        else
        {
            curl_easy_setopt(m_CurlHandle, CURLOPT_POSTFIELDS, requestBody.GetData());
            curl_easy_setopt(m_CurlHandle, CURLOPT_POSTFIELDSIZE, requestBody.GetSize());
        }
    }
	
    
	curl_easy_setopt(m_CurlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_CurlHandle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(m_CurlHandle, CURLOPT_ERRORBUFFER, mErrorBuffer);
    curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEFUNCTION, WebServices_CurlImpl::responseCallBackProc_s);
    curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(m_CurlHandle, CURLOPT_HTTPHEADER, mHTTPHeader);
    curl_easy_setopt(m_CurlHandle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(m_CurlHandle, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(m_CurlHandle, CURLOPT_PROGRESSFUNCTION, WebServices_CurlImpl::progressCallBackProc_s);
    
    if(mStoreHeaders)
    {
        curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEHEADER, this);
        curl_easy_setopt(m_CurlHandle, CURLOPT_HEADERFUNCTION, WebServices_CurlImpl::headerCallBackProc_s);
    }
    
	//Scedule the task now!
    WSEngineSharedPtr thisPtr = shared_from_this();
    mUserData = userData;
	mTasksPool.enqueue<int>([this, userData, requestBody, thisPtr]()->int{
		auto ret = ExecuteRequest(userData);
        return ret;
  
	}, mTaskPriority);
}

int WebServices_CurlImpl::progressCallBackProc_s(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
    WebServices_CurlImpl *pThisInstance = (WebServices_CurlImpl *)(clientp);
    if(pThisInstance)
        return pThisInstance->progressCallBackProc(clientp, dltotal, dlnow, ultotal, ulnow);
    else
        return 0;
}

int WebServices_CurlImpl::progressCallBackProc(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if(!dltotal)
    {
        dltotal = ultotal;
        dlnow = ulnow;
    }
    
    Utils::Log(UString("Curl progress: ") + UString(dlnow) + UString("/") + UString(dltotal));
    if(mProgressCallback)
    {
        
        return mProgressCallback(dltotal, dlnow, mUserData);
    }
    else
        return 0;
    
}

size_t WebServices_CurlImpl::responseCallBackProc_s( char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t sizeHandled = 0;
	WebServices_CurlImpl *pThisInstance = (WebServices_CurlImpl *)(userdata);
	if(pThisInstance)
		sizeHandled = pThisInstance->responseCallBackProc(ptr, size*nmemb);

	return sizeHandled;
}

size_t WebServices_CurlImpl::responseCallBackProc(const char *ptr, size_t size)
{
	mResponseData.insert(mResponseData.end(), (const unsigned char *)ptr, (const unsigned char *)ptr + size);
	return size;
}

int WebServices_CurlImpl::ExecuteRequest(const UserDataType_t &userData)
{
	int responseCode = 0;
	auto res = curl_easy_perform(m_CurlHandle);
    char *reqURL = NULL;
    curl_easy_getinfo(m_CurlHandle, CURLINFO_EFFECTIVE_URL, &reqURL);
	if(res == CURLE_OK)
	{
		curl_easy_getinfo(m_CurlHandle, CURLINFO_RESPONSE_CODE, &responseCode);
        
        if(responseCode == kHTTPResponseOK && mDataCallbackProc)
        {
            Utils::Log(UString("Request completed: ") + UString(reqURL) + UString(", HTTP result: ") +
                       UString(responseCode) + UString("\nRaw Response data size: ") + UString(mResponseData.size()));
            
            mDataCallbackProc(responseCode, userData, mResponseData.data(), mResponseData.size(), mHeadersList);
        }
        else 
        {
            UTF8String responseString;
            responseString.assign((const char *)mResponseData.data(), mResponseData.size());
            Utils::Log(UString("Request completed: ") + UString(reqURL) + UString(", HTTP result: ") +
                       UString(responseCode) + UString("\nResponse: ") + UString::fromUtf8(responseString.c_str()));
            if(mCallBackProc)
                mCallBackProc(responseCode, userData, UString::fromUtf8(responseString.c_str()), mHeadersList);
        }
        
	}
	else
	{
        Utils::Log(UString("Request Failed: ") + UString(reqURL) + UString(", CURL result: ") +
                   UString(res));
        
        if(mCallBackProc)
            mCallBackProc(res, userData, UString::fromUtf8(mErrorBuffer), mHeadersList);
	}
	curl_easy_cleanup(m_CurlHandle);
	m_CurlHandle = NULL;

	return 0;
}

size_t WebServices_CurlImpl::headerCallBackProc_s( char *ptr, size_t size, size_t nmemb, void *userdata)
{
    WebServices_CurlImpl *pThisInstance = (WebServices_CurlImpl *)(userdata);
	if(pThisInstance)
		pThisInstance->headerCallBackProc(ptr, size*nmemb);
    
    return size*nmemb;
}

void WebServices_CurlImpl::headerCallBackProc( char *ptr, size_t size)
{
    std::string header(ptr, size);
    mHeadersList.push_back(header);
}