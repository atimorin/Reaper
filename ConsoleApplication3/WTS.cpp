#include "stdafx.h"

void WTS::EnumerateServers()
{
	//PWTS_SERVER_INFO serverInfo;
	PSERVER_INFO_100 serverInfo;
	DWORD entriesRead;
	DWORD totalEntries;
	DWORD i;
	DWORD ret;
	/*wprintf(L"domain: %s\n", Util::DomainFromIP(Config::machine));
	if(WTSEnumerateServers(Util::DomainFromIP(Config::machine), 0, 1, &serverInfo, &totalEntries) != 0)
	{
		Util::Error(GetLastError(), L"WTSEnumerateServers()");
	}*/
	if((ret = NetServerEnum(NULL, 100, (LPBYTE *)&serverInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, SV_TYPE_TERMINALSERVER, Util::DomainFromIP(Config::machine), NULL)) != NERR_Success)
	{
		Util::Error(ret, L"NetServerEnum()");
	}

	for(i = 0; i < totalEntries; i++)
	{
		wprintf(L"Entry: %s\n", serverInfo[i].sv100_name);
	}
	wprintf(L"Total entries: %d\n", totalEntries);
}

void WTS::EnumerateProcesses()
{
	HANDLE server;
	DWORD totalEntries;
	PWTS_PROCESS_INFO processInfo;
	DWORD i;

	if((server = WTSOpenServer(Config::machine)) == NULL)
	{
		Util::Error(GetLastError(), L"WTSOpenServer()");
	}

	if(WTSEnumerateProcesses(server, 0, 1, &processInfo, &totalEntries) != 0)
	{
		DWORD err = GetLastError();
		wprintf(L"ret = %x\n", err);
	}

	for(i = 0; i < totalEntries; i++)
	{
		wprintf(L"process: %s\n", processInfo[i].pProcessName);
		wprintf(L"session: %d\n", processInfo[i].SessionId);
		wprintf(L"SID: %s\n", processInfo[i].pUserSid);
	}
}