#include "stdafx.h"

BOOL UserManager::EnumerateUserNames(LPCWSTR domain)
{
	LPUSER_INFO_0 userInfo = NULL;
	DWORD level = 0;
	DWORD entriesRead = 0;
	DWORD entriesTotal = 0;
	DWORD resumeHandle;
	DWORD index = 0;
	DWORD ret;
	CSV csv;
	if((ret = NetUserEnum(domain, level, FILTER_NORMAL_ACCOUNT, (LPBYTE *)&userInfo, MAX_PREFERRED_LENGTH, &entriesRead, &entriesTotal, &resumeHandle)) != NERR_Success)
	{
		Util::Error(ret, L"NetUserEnum():");
		NetApiBufferFree(userInfo);
		return false;
	}
	else
	{
		Util::PrintLine();
		if(Config::csvOutput)
		{
			csv.AddRow(L"Name");
		}
		for(index = 0; index < entriesRead; index++)
		{
			wprintf(L"%s\n", userInfo[index].usri0_name);
			if(Config::csvOutput)
			{
				csv.AddRow(userInfo[index].usri0_name);
			}
		}
		if(Config::csvOutput)
		{
			csv.PrintToFile();
			csv.Clear();
		}
		Util::PrintLine();
		NetApiBufferFree(userInfo);
		return true;
	}
}

void UserManager::EnumerateUserInformation(LPCWSTR domain)
{
	LPUSER_INFO_2 userInfo = NULL;
	DWORD level = 2;
	DWORD entriesRead = 0;
	DWORD entriesTotal = 0;
	DWORD resumeHandle;
	DWORD index = 0;
	DWORD ret;
	
	if((ret = NetUserEnum(domain, level, FILTER_NORMAL_ACCOUNT, (LPBYTE *)&userInfo, MAX_PREFERRED_LENGTH, &entriesRead, &entriesTotal, &resumeHandle)) != NERR_Success)
	{
		Util::Error(ret, L"NetUserEnum():");
	}
	else
	{
		Util::PrintLine();
		for(index = 0; index < entriesRead; index++)
		{
			users.push_back(new User());
			users[index]->sidString = L"N/A";
			// Lots of stuff to pack into our user object.
			if(userInfo[index].usri2_name != NULL && lstrlen(userInfo[index].usri2_name) > 0)
			{
				users[index]->accountName = userInfo[index].usri2_name;
			}

			if(userInfo[index].usri2_full_name != NULL && lstrlen(userInfo[index].usri2_full_name) > 0)
			{
				users[index]->fullName = userInfo[index].usri2_full_name;
			}

			if(userInfo[index].usri2_home_dir != NULL && lstrlen(userInfo[index].usri2_home_dir) > 0)
			{
				users[index]->homePath = userInfo[index].usri2_home_dir;
			}

			if(userInfo[index].usri2_script_path != NULL && lstrlen(userInfo[index].usri2_script_path) > 0)
			{
				users[index]->scriptPath = userInfo[index].usri2_script_path;
			}

			if(userInfo[index].usri2_workstations != NULL && lstrlen(userInfo[index].usri2_workstations) > 0)
			{
				users[index]->workStations = userInfo[index].usri2_workstations;
			}

			if(userInfo[index].usri2_last_logon != NULL && userInfo[index].usri2_last_logon > 0 && userInfo[index].usri2_last_logon != UNINITIALIZED)
			{
				users[index]->lastLogin = userInfo[index].usri2_last_logon;
			}
			else
			{
				users[index]->lastLogin = 0;
			}

			if(userInfo[index].usri2_last_logoff != NULL && userInfo[index].usri2_last_logoff > 0 && userInfo[index].usri2_last_logoff != UNINITIALIZED)
			{
				users[index]->lastLogout = userInfo[index].usri2_last_logoff;
			}
			else
			{
				users[index]->lastLogout = 0;
			}
			if(userInfo[index].usri2_num_logons != NULL && userInfo[index].usri2_num_logons > 0 && userInfo[index].usri2_num_logons != UNINITIALIZED)
			{
				users[index]->loginCount = userInfo[index].usri2_num_logons;
			}
			else
			{
				users[index]->loginCount = 0;
			}

			if(userInfo[index].usri2_usr_comment != NULL && lstrlen(userInfo[index].usri2_usr_comment) > 0)
			{
				users[index]->userComment = userInfo[index].usri2_comment;
			}

			users[index]->userPrivs = userInfo[index].usri2_priv;

			// Pack groups for user
			PGROUP_INFO_0 groupInfo;
			DWORD numEntries;
			DWORD numTotal;

			if((ret = NetUserGetGroups(Config::machine, users[index]->accountName.c_str(), 0, (LPBYTE *)&groupInfo, MAX_PREFERRED_LENGTH, &numEntries, &numTotal)) != NERR_Success)
			{
				Util::Error(ret, L"NetUserGetGroups()");
			}
			else
			{
				DWORD j;
				for(j = 0; j < numEntries; j++)
				{
					users[index]->groups.push_back(new std::wstring(groupInfo[j].grpi0_name));
				}
			}
			NetApiBufferFree(groupInfo);
			users[index]->PrettyPrint();
		}
		//wprintf(L"Entries read: %d\n", entriesRead);
	}
	Util::Notice(L"User enumeration complete.\n\n");
	NetApiBufferFree(userInfo);
}

void UserManager::DumpToCSV()
{
	DWORD i;
	CSV csv;

	csv.AddRow(L"Account name, SID, Full name, Home path, Script path, Groups");

	for(i = 0; i < users.size(); i++)
	{
		std::wstring csvString;
		if(lstrlen(users[i]->accountName.c_str()) > 0)
		{
			csvString.append(users[i]->accountName);
			csvString.append(L",");
		}
		else
		{
			csvString.append(L"N/A,");
		}

		//csvString.append(users[i]->sidString);
		csvString.append(L"N/A");
		csvString.append(L",");

		if(lstrlen(users[i]->fullName.c_str()) > 0)
		{
			csvString.append(L"\"");
			csvString.append(users[i]->fullName);
			csvString.append(L"\",");
		}
		else
		{
			csvString.append(L"N/A,");
		}

		if(lstrlen(users[i]->homePath.c_str()) > 0)
		{
			csvString.append(users[i]->homePath);
			csvString.append(L",");
		}
		else
		{
			csvString.append(L"N/A,");
		}

		if(lstrlen(users[i]->scriptPath.c_str()) > 0)
		{

			csvString.append(users[i]->scriptPath);
			csvString.append(L",");
		}
		else
		{
			csvString.append(L"N/A,");
		}
		if(users[i]->groups.size() > 0)
		{
			DWORD j;
			csvString.append(L"\"");
			for(j = 0; j < users[i]->groups.size(); j++)
			{
				if(j < users[i]->groups.size()-1)
				{
					csvString.append(users[i]->groups[j]->c_str());
					csvString.append(L",");
				}
				else
				{
					csvString.append(users[i]->groups[j]->c_str());
				}
			}
			csvString.append(L"\"");
		}
		else
		{
			csvString.append(L"N/A,");
		}
		csv.AddRow((LPWSTR)csvString.c_str());
	}
	csv.PrintToFile();
	csv.Clear();
}

void UserManager::Clear()
{
	DWORD i;
	for(i = 0; i < users.size(); i++)
	{
		DWORD j;
		for(j = 0; j < users[i]->groups.size(); j++)
		{
			delete(users[i]->groups[j]);
		}
		users[i]->groups.clear();
		delete(users[i]);
	}
}