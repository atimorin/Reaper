#pragma once

#include "stdafx.h"

class AD
{
public:
	static std::wstring host;
	static std::wstring user;
	static std::wstring pass;
	static std::wstring dn;
	static std::wstring filter;

	static UserManager *userManager;
	static GroupManager *groupManager;
	static ServerManager *serverManager;
	static LDAP *ldap;
	static SecPkgContext_CertInfo sslInfo;

	static bool Init();
	static bool InitSSL();
	static bool Connect();
	static bool Login();
	static void SetUser(LPWSTR s);
	static void SetPass(LPWSTR s);
	static LPWSTR GetUser();
	static LPWSTR GetPass();
	static bool Disconnect();
	static void SetHost(LPWSTR s);
	static LPWSTR GetHost();
	static void SetFilter(LPWSTR s);
	static LPWSTR GetFilter();
	static void SetDN(LPWSTR s);
	static void GetDN();
	static void SetUserManager(UserManager *u);
	static UserManager *GetUserManager();
	static void SetGroupManager(GroupManager *g);
	static GroupManager *GetGroupManager();
	static void SetServerManager(ServerManager *s);
	static ServerManager *GetServerManager();
	static void EnumerateUsers();
};