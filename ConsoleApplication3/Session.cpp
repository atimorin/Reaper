#include "stdafx.h"

void Session::ConnectWNet(LPWSTR user, LPWSTR pass, LPWSTR host)
{
	std::wstring string;
	if(host[0] != L'\\' && host[1] != L'\\')
	{
		string.append(L"\\\\");
	}
	string.append(host);
	NETRESOURCE netResource;
	netResource.lpLocalName = NULL;
	netResource.lpRemoteName = (LPWSTR)string.c_str();
	netResource.dwType = RESOURCETYPE_ANY;
	netResource.lpProvider = NULL;

	DWORD ret;
	
	if((ret = WNetAddConnection2(&netResource, pass, user, CONNECT_UPDATE_PROFILE)) != NO_ERROR)
	{
		Util::Error(ret, L"WNetAddConnection2()");
		exit(1);
	}
}

void Session::DisconnectWNet(LPWSTR host)
{
	DWORD ret;
	std::wstring string;
	if(host[0] != L'\\' && host[1] != L'\\')
	{
		string.append(L"\\\\");
	}
	string.append(host);
	if((ret = WNetCancelConnection2((LPWSTR)string.c_str(), CONNECT_UPDATE_PROFILE, true)) != NO_ERROR)
	{
		Util::Error(ret, L"WNETCancelConnection2():");
	}
}

void Session::ConnectLSA(LPWSTR user, LPWSTR pass, LPWSTR host)
{
	DWORD ret = 0;
}

void Session::ConnectLDAP()
{
}

void Session::DisconnectLDAP()
{
}