#include "stdafx.h"

UserManager *LSA::userManager = NULL;
GroupManager *LSA::groupManager = NULL;
ServerManager *LSA::serverManager = NULL;

void LSA::OpenPolicy(Session *session, LPWSTR host)
{
	DWORD ret;
	HANDLE handle = NULL;
	LSA_OBJECT_ATTRIBUTES attributes;
	LSA_UNICODE_STRING systemName;

	ZeroMemory(&attributes, sizeof(attributes));
	ZeroMemory(&systemName, sizeof(systemName));

	systemName.Buffer = host;
	systemName.Length = systemName.MaximumLength = lstrlen(host) * sizeof(WCHAR);

	if((ret = LsaOpenPolicy(&systemName, &attributes, POLICY_LOOKUP_NAMES, &handle)) != 0)
	{
		//wprintf(L"ret = %x\n", ret);
		Util::Error(ret, L"LsaOpenPolicy()");
		exit(1);
	}
	else
	{
		session->lsa = handle;
	}

}

void LSA::EnumerateSIDs(Session *session, SID_NAME_USE sidType)
{
	//TODO: make this method suck less.
	//UPDATE: feeling better about this method, but it could still probably suck less.
	//CSV csv;
	DWORD ret;
	LSA_UNICODE_STRING domainName;
	PLSA_TRANSLATED_SID2 sid;
	PLSA_REFERENCED_DOMAIN_LIST domains;
	DWORD sidStringSize = sizeof(WCHAR) * 128;
	DWORD i = 0;

	Util::LsaUnicodeString(Util::DomainFromIP(Config::machine), &domainName);
	Util::PrintLine();

	if((ret = LsaLookupNames2(session->lsa, NULL, 1, &domainName, &domains, &sid)) != 0)
	{
		wprintf(L"ret = %x\n", ret);
		Util::Error(ret, L"LsaLookupNames2()");
		LsaFreeMemory(domains);
		LsaFreeMemory(sid);
	}
	else
	{
		// Free domains so we can re-use it with LsaLookupSids()
		LsaFreeMemory(domains);

		LPWSTR domainSid = NULL;

		if(ConvertSidToStringSid(sid[0].Sid, &domainSid) == 0)
		{
			Util::Error(GetLastError(), L"ConvertSidToStringSid()");
		}

		// Enumeration loop.
		for(i = 0; i < Config::numBruteTries; i++)
		{
			PSID newSid;
			PLSA_TRANSLATED_NAME translatedName;
			std::wstring newSidString;
			WCHAR num[32];
			_itow_s(i, num, 10);

			newSidString.append(domainSid);
			newSidString.append(L"-");
			newSidString.append(num); 

			if(ConvertStringSidToSid(newSidString.c_str(), &newSid) == 0)
			{
				Util::Error(GetLastError(), L"ConvertStringSidToSid()");
			}
			// Error checking on this would be great except for the fact that failing to
			// find anything associated with the SID throws a failure condition, which
			// just results in needless noise in the form of error reporting.
			LsaLookupSids(session->lsa, 1, &newSid, &domains, &translatedName);

			if(translatedName->Use == sidType)
			{
				std::wstring accountName;
				accountName.append(translatedName->Name.Buffer, translatedName->Name.Length / sizeof(WCHAR));

				// If the account name ends in a dollar sign then it is a machine account.
				if(accountName.c_str()[accountName.length()-1] == 0x24 && Config::machineAccountHack)
				{
					//wprintf(L"Name: %s\nSID: %s\n\n", accountName.c_str(), newSidString.c_str());
					//Util::PrintLine();
				}
				else if(accountName.c_str()[accountName.length()-1] != 0x24 && !Config::machineAccountHack)
				{
					//Util::Print(L"Name: %s\nSID: %s\n\n", accountName.c_str(), newSidString.c_str());
					//Util::PrintLine();

				}
				// We've enumerated a SID, time to sort it into the right place.
				switch(translatedName->Use)
				{
				case SidTypeUser:
					if(accountName.c_str()[accountName.length()-1] == L'$' && Config::machineAccountHack)
					{
						LSA::AddServer((LPWSTR)accountName.c_str(), (LPWSTR)newSidString.c_str());
					}
					else if(accountName.c_str()[accountName.length()-1] != L'$' && !Config::machineAccountHack)
					{
						LSA::AddUser((LPWSTR)accountName.c_str(), (LPWSTR)newSidString.c_str());
					}
					break;
				case SidTypeGroup:
					LSA::AddGroup((LPWSTR)accountName.c_str(), (LPWSTR)newSidString.c_str());
				default:
					break;
				}
			}
			// Free allocated stuff
			LocalFree(newSid);
			LsaFreeMemory(translatedName);
			LsaFreeMemory(domains);
		}
		LsaFreeMemory(sid);

		// Free buffers that were allocated.
		// Be sure to use the correct corresponding free call or heap corruption ensues.

		LocalFree(domainSid);
		LsaFreeMemory(domains);
	}
}

void LSA::SetUserManager(UserManager *u)
{
	userManager = u;
}

UserManager *LSA::GetUserManager()
{
	return userManager;
}

void LSA::SetGroupManager(GroupManager *g)
{
	groupManager = g;
}

GroupManager *LSA::GetGroupManager()
{
	return groupManager;
}

void LSA::SetServerManager(ServerManager *s)
{
	serverManager = s;
}

ServerManager *LSA::GetServerManager()
{
	return serverManager;
}

void LSA::AddUser(LPWSTR accountName, LPWSTR accountSID)
{
	LPUSER_INFO_2 userInfo;
	LPGROUP_INFO_0 groupInfo;
	DWORD entriesRead;
	DWORD totalEntries;
	DWORD ret;
	DWORD i;

	if((ret = NetUserGetInfo(Config::machine, (LPCWSTR)accountName, 2, (LPBYTE *)&userInfo)) != NERR_Success)
	{
		Util::Error(ret, L"NetUserGetInfo()");
		NetApiBufferFree(userInfo);
	}
	else
	{
		User *user = new User();

		user->accountName = userInfo->usri2_name;
		user->sidString = accountSID;
		user->fullName = userInfo->usri2_full_name;
		user->homePath = userInfo->usri2_home_dir;
		user->scriptPath = userInfo->usri2_script_path;
		user->workStations = userInfo->usri2_workstations;
		user->lastLogin = userInfo->usri2_last_logon;
		user->lastLogout = userInfo->usri2_last_logoff;

		if((ret = NetUserGetGroups(Config::machine, (LPCWSTR)accountName, 0, (LPBYTE *)&groupInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries)) != NERR_Success)
		{
			Util::Error(ret, L"NetUserGetGroups()");
			NetApiBufferFree(groupInfo);
		}
		else
		{
			for(i = 0; i < entriesRead; i++)
			{
				user->groups.push_back(new std::wstring(groupInfo[i].grpi0_name));
			}
			userManager->users.push_back(user);
			user->PrettyPrint();
			NetApiBufferFree(groupInfo);
		}
		NetApiBufferFree(userInfo);
	}
}

void LSA::AddGroup(LPWSTR groupName, LPWSTR groupSID)
{
	DWORD ret;
	DWORD i;
	LPGROUP_USERS_INFO_0 groupInfo;
	DWORD totalEntries;
	DWORD entriesRead;

	Group *group = new Group();

	group->name = groupName;

	if((ret = NetGroupGetUsers(Config::machine, groupName, 0, (LPBYTE *)&groupInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, NULL)) != NERR_Success)
	{
		Util::Error(ret, L"NetGroupGetUsers()");
	}

	if(entriesRead > 0)
	{
		for(i = 0; i < entriesRead; i++)
		{
			if(groupInfo[i].grui0_name[lstrlen(groupInfo[i].grui0_name)-1] != L'$')
			{
				group->users.push_back(new std::wstring(groupInfo[i].grui0_name));
			}
		}
	}
	NetApiBufferFree(groupInfo);
	group->PrettyPrint();
	groupManager->groups.push_back(group);
}

void LSA::AddServer(LPWSTR serverName, LPWSTR serverSID)
{
	DWORD i;
	DWORD ret;
	DWORD entriesTotal;
	DWORD entriesRead;
	PSHARE_INFO_1 shareInfo = NULL;
	HANDLE pingHandle;
	hostent *host;
	WSADATA wsaData;
	in_addr addr;
	std::wstring machineName;
	
	// Microsoft reserves the $ at the end of account names
	// to designate them as machine accounts, yet in their
	// infinite wisdom made it so that if you resolve that exact
	// machine name, it will be invalid. Therefor you have to
	// lop off the trailing $ or windows complains. Thanks
	// for documenting that guys! /s
	//machineName.erase(machineName.end());
	machineName.append(serverName);
	if(machineName.length() > 0)
	{
		// Chop off that pesky dollar sign.
		machineName.resize(machineName.length()-1);
	}

	if((ret = NetShareEnum((LPWSTR)machineName.c_str(), 1, (LPBYTE *)&shareInfo, MAX_PREFERRED_LENGTH, &entriesRead, &entriesTotal, NULL)) != NERR_Success)
	{
		Util::Error(ret, L"NetShareEnum()");
		wprintf(L"ret = %x\n", ret);
	}

	Server *server = new Server();
	server->name = machineName;
	for(i = 0; i < entriesRead; i++)
	{
		server->shares.push_back(new std::wstring(shareInfo[i].shi1_netname));
	}
	server->PrettyPrint();
	serverManager->servers.push_back(server);
	NetApiBufferFree(shareInfo);

}