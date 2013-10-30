#pragma once
#include "stdafx.h"

class GroupManager;

class LSA
{
private:
	static UserManager *userManager;
	static GroupManager *groupManager;
	static ServerManager *serverManager;

public:
	static void OpenPolicy(Session *session, LPWSTR host);
	static DWORD LookupNames(Session *session, PLSA_UNICODE_STRING names, DWORD count, PLSA_TRANSLATED_SID2 sidList);
	static void EnumerateSIDs(Session *session, SID_NAME_USE sidType);
	static void SetUserManager(UserManager *u);
	static UserManager *GetUserManager();
	static void SetGroupManager(GroupManager *g);
	static GroupManager *GetGroupManager();
	static void SetServerManager(ServerManager *s);
	static ServerManager *GetServerManager();
	static void AddUser(LPWSTR accountName, LPWSTR accountSID);
	static void AddGroup(LPWSTR groupName, LPWSTR groupSID);
	static void AddServer(LPWSTR serverName, LPWSTR serverSID);
};