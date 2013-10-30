#include "stdafx.h"

void GroupManager::EnumerateGroups(LPWSTR serverName)
{
	GROUP_INFO_0 *groupInfo = NULL;
	DWORD entriesRead;
	DWORD totalEntries;
	DWORD ret;
	DWORD i;
	CSV csv;

	Util::PrintLine();

	if((ret = NetGroupEnum(serverName, 0, (LPBYTE *)&groupInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, NULL)) != NERR_Success)
	{
		Util::Error(ret, L"NetGroupEnum()");
		wprintf(L"ret = %x", ret);
	}
	else
	{
		for(i = 0; i < entriesRead; i++)
		{
			PGROUP_USERS_INFO_0 userInfo;
			DWORD numEntries;
			DWORD numTotal;
			DWORD j;
			//Push back new group object.
			groups.push_back(new Group());
			wprintf(L"Group: %s\n", groupInfo[i].grpi0_name);
			groups[i]->name.append(groupInfo[i].grpi0_name);
			wprintf(L"Users: ");
			//Enumerate group users.
			if((ret = NetGroupGetUsers(serverName, groupInfo[i].grpi0_name, 0, (LPBYTE *)&userInfo, MAX_PREFERRED_LENGTH, &numEntries, &numTotal, NULL)) != NERR_Success)
			{
				Util::Error(ret, L"NetGroupGetUsers()");
			}
			else
			{
				if(numEntries <= 0)
				{
					wprintf(L"N/A\n");
				}
				else
				{
					for(j = 0; j < numEntries; j++)
					{
						groups[i]->users.push_back(new std::wstring(userInfo[j].grui0_name));
						if(j < numEntries-1)
						{
							wprintf(L"%s ", userInfo[j].grui0_name);
						}
						else
						{
							wprintf(L"%s\n", userInfo[j].grui0_name);
						}
					}
				}
			}
			NetApiBufferFree(userInfo);
			wprintf(L"\n");
			Util::PrintLine();
		}
	}
	Util::Notice(L"Group enumeration complete.\n\n");
	NetApiBufferFree(groupInfo);
}

void GroupManager::DumpToCSV()
{
	CSV csv;
	DWORD i;
	DWORD j;

	csv.AddRow(L"Group Name,Members");

	for(i = 0; i < groups.size(); i++)
	{
		std::wstring string;
		string.append(groups[i]->name);
		string.append(L",");
		string.append(L"\"");
		for(j = 0; j < groups[i]->users.size(); j++)
		{
			if(j == groups[i]->users.size()-1)
			{
				string.append(groups[i]->users[j]->c_str());
			}
			else
			{
				string.append(groups[i]->users[j]->c_str());
				string.append(L",");
			}
		}
		string.append(L"\"");
		csv.AddRow((LPWSTR)string.c_str());
	}


	csv.PrintToFile();
	csv.Clear();
}

void GroupManager::Clear()
{
	DWORD i;

	for(i = 0; i < groups.size(); i++)
	{
		delete(groups[i]);
	}
	groups.clear();
}