// TestProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WebServices_CurlImpl.h"
#include "WebServicesInterface.h"
#include "Ustring.h"
#include <sstream>
using namespace NewGroup::Core;

void CallBack(int, const WebServicesInterface::UserDataType_t& userData, const UString& response)
{
}

UString GenerateTimestamp()
{
	SYSTEMTIME sm;	
	FILETIME fm;
	LARGE_INTEGER currentTime, baseTime;

	GetSystemTime(&sm);
	SystemTimeToFileTime(&sm, &fm);
	currentTime.HighPart = fm.dwHighDateTime;
	currentTime.LowPart = fm.dwLowDateTime;

	memset(&sm, 0, sizeof(sm));
	sm.wDay = 1;
	sm.wMonth = 1;
	sm.wYear = 1970;
	sm.wSecond = 1;

	SystemTimeToFileTime(&sm, &fm);

	baseTime.HighPart = fm.dwHighDateTime;
	baseTime.LowPart = fm.dwLowDateTime;

	long long secondsLapsed = currentTime.QuadPart - baseTime.QuadPart;
	std::wstringstream strStream;
	strStream << (secondsLapsed)/10000000LL;
	return UString(strStream.str());
}

UString GenerateUUID()
{
	
	RPC_WSTR buffer = NULL;
	UUID uuid;
	UuidCreate(&uuid);
	UuidToStringW(&uuid, &buffer);

	UString uuidStr((wchar_t *)buffer);
	RpcStringFreeW(&buffer);

	return uuidStr;
}

int _tmain(int argc, _TCHAR* argv[])
{
	WebServices_CurlImpl *p = new WebServices_CurlImpl(std::bind(CallBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	WebServicesInterface::UserDataType_t userData;

	WebServicesInterface::ParametersType_t params;
	params[UString(L"oauth_consumer_key")] = UString(L"337sy7vdvcycivz");
	params[UString(L"oauth_signature_method")] = UString(L"PLAINTEXT");
	params[UString(L"oauth_signature")] = UString(L"ez00yzm7xatak7s") + UString(L"&");
	params[UString(L"oauth_timestamp")] = GenerateTimestamp();
	params[UString(L"oauth_nonce")] = GenerateUUID();
	//UuidCreate(
	//params.insert(UString(), UString());
	/*params.insert(UString(L"oauth_signature_method"), UString(L""));
	params.insert(UString(L"oauth_signature"), UString(L""));
	params.insert(UString(L"oauth_timestamp"), UString(L""));
	params.insert(UString(L"oauth_nonce"), UString(L""));*/

	p->SendGetRequest(UString(L"https://api.dropbox.com/1/oauth/request_token"), params, userData);
	Sleep(30);
	return 0;
}

