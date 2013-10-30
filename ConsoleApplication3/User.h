#pragma once
#include "stdafx.h"

class User
{
public:
	std::wstring accountName;
	std::wstring fullName;
	std::wstring homePath;
	std::wstring scriptPath;
	std::wstring userComment;
	std::wstring workStations;
	std::wstring sidString;

	DWORD userPrivs;
	DWORD authFlags;
	DWORD lastLogin;
	DWORD lastLogout;
	DWORD loginCount;

	std::vector<std::wstring *> groups;

	User()
	{
		userPrivs = NULL;
		authFlags = NULL;
		lastLogin = NULL;
		lastLogout = NULL;
		loginCount = NULL;
	}

	void PrettyPrint();
};