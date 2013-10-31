#include "stdafx.h"

void ServerManager::EnumerateServerInformation(LPWSTR domain)
{
	LPSERVER_INFO_101 serverInfo;
	DWORD entriesRead = 0;
	DWORD totalEntries = 0;
	DWORD resumeHandle = 0;
	NET_API_STATUS ret = 0;
	LPWSTR domainName = (LPWSTR)Util::DomainFromIP(Config::machine);
	
	ret = NetServerEnum(NULL, 101, (LPBYTE *)&serverInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, SV_TYPE_ALL, domainName, &resumeHandle);
	if(ret == NERR_Success)
	{
		DWORD i;
		Util::PrintLine();

		for(i = 0; i < entriesRead; i++)
		{
			PSHARE_INFO_1 shareInfo;
			DWORD numEntries;
			DWORD total;
			DWORD j;

			servers.push_back(new Server());
			servers[i]->name.append(serverInfo[i].sv101_name);
			servers[i]->type = serverInfo[i].sv101_type;

			wprintf(L"Name: %s\n", servers[i]->name.c_str());
			wprintf(L"Type: %s\n", Util::ServerFlagsToString(servers[i]->type));
			wprintf(L"Shares: ");

			if((ret = NetShareEnum(serverInfo[i].sv101_name, 1, (LPBYTE *)&shareInfo, MAX_PREFERRED_LENGTH, &numEntries, &total, NULL)) != NERR_Success)
			{
				Util::Error(ret, L"NetShareEnum()");
			}
			for(j = 0; j < numEntries; j++)
			{
				servers[i]->shares.push_back(new std::wstring(shareInfo[j].shi1_netname));
				if(j < numEntries-1)
				{
					wprintf(L"%s, ", shareInfo[j].shi1_netname);
				}
				else
				{
					wprintf(L"%s", shareInfo[j].shi1_netname);
				}
			}
			wprintf(L"\n\n");
			Util::PrintLine();
			NetApiBufferFree(shareInfo);
		}
	}
	else
	{
		Util::Error(ret, L"NetServerEnum():");
		wprintf(L"Ret : %x\n", ret);
	}
	NetApiBufferFree(serverInfo);
	Util::Notice(L"Server enumeration complete.\n\n");
}

void ServerManager::DumpToCSV()
{
	CSV csv;
	DWORD i;
	DWORD j;

	std::wstring csvString;

	csv.AddRow(L"Name, Type, Shares");

	for(i = 0; i < servers.size(); i++)
	{
		csvString.append(servers[i]->name.c_str());
		csvString.append(L",\"");
		csvString.append(Util::ServerFlagsToString(servers[i]->type));
		csvString.append(L"\",");

		if(servers[i]->shares.size() <= 0)
		{
			csvString.append(L"N/A");
		}
		else
		{
			csvString.append(L"\"");
			for(j = 0; j < servers[i]->shares.size(); j++)
			{
				csvString.append(servers[i]->shares[j]->c_str());
				csvString.append(L",");
			}
			csvString.append(L"\"");

		}
		csv.AddRow((LPWSTR)csvString.c_str());
		csvString.clear();
	}

	csv.PrintToFile();
	csv.Clear();
}

void ServerManager::Clear()
{
	DWORD i;
	for(i = 0; i < servers.size(); i++)
	{
		delete(servers[i]);
	}
	servers.clear();
}