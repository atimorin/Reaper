#include "stdafx.h"

// This file contains convenience functions, becase winapi is inconvenient.

WCHAR *Util::ServerFlagsToString(DWORD flags)
{
	switch((flags & 0xff0000ff))
	{
	case 0x01:
		return L"Workstation";
	case 0x02:
		return L"Server";
	case 0x03:
		return L"Workstation, Server";
	case 0x04:
		return L"SQL Server";
	case 0x05:
		return L"Workstation, SQL Server";
	case 0x06:
		return L"Server, SQL Server";
	case 0x07:
		return L"Workstation, Server, SQL Server";
	case 0x08:
		return L"Domain Controller";
	case 0x09:
		return L"Workstation, Domain Controller";
	case 0x10:
		return L"Server, Domain Controller";
	case 0x20000000:
		return L"Terminal Server";
	case 0x20000001:
		return L"Terminal Server, Workstation";
	case 0x20000002: 
		return L"Terminal Server, Server";
	case 0x20000003:
		return L"Terminal Server, Workstation, Server";
	case 0x20000004:
		return L"Terminal Server, SQL Server";
	default:
		return L"Unknown";
	}
}

WCHAR *Util::NetErrorToString(DWORD error)
{
	switch(error)
	{
	case ERROR_ACCESS_DENIED:
		return L"Access denied.";
	case ERROR_INVALID_LEVEL:
		return L"Incorrect call level.";
	case ERROR_MORE_DATA:
		return L"Buffer too small for total data.";
	case ERROR_INVALID_PARAMETER:
		return L"One or more parameters are invalid.";
	case ERROR_NO_BROWSER_SERVERS_FOUND:
		return L"No browser servers found.";
	case ERROR_NOT_SUPPORTED:
		return L"Request not supported.";
	case ERROR_NOT_ENOUGH_MEMORY:
		return L"Insufficient memory.";
	case ERROR_ALREADY_ASSIGNED:
		return L"Already connected to resource.";
	case ERROR_BAD_DEV_TYPE:
		return L"Local device and network resource don't match.";
	case ERROR_BAD_DEVICE:
		return L"Invalid device.";
	case ERROR_BAD_NET_NAME:
		return L"Network cannot be found.";
	case ERROR_BAD_PROFILE:
		return L"User profile incorrectly formatted.";
	case ERROR_BAD_PROVIDER:
		return L"Invalid network provider name.";
	case ERROR_BAD_USERNAME:
		return L"Invalid user name.";
	case ERROR_INVALID_PASSWORD:
		return L"Invalid password";
	case ERROR_LOGON_FAILURE:
		return L"Incorrect user name or password.";
	case ERROR_BUSY:
		return L"The provider is busy.";
	case ERROR_NO_NETWORK:
		return L"The network is unavailable.";
	case NERR_InvalidComputer:
		return L"Invalid computer name.";
	case NERR_BufTooSmall:
		return L"Buffer too small to contain an entry.";
	case NERR_RemoteErr:
		return L"No data returned by the server.";
	case NERR_ServerNotStarted:
		return L"Server service not started.";
	case NERR_WkstaNotStarted:
		return L"Workstation service not started.";
	default:
		return L"Unknown error.\n";
	}
}

void Util::Notice(LPWSTR string, ...)
{
	va_list argList;
	va_start(argList, string);
	SetConsoleColor(LIGHT_GREEN);
	wprintf(L"[+] ");
	vwprintf(string, argList);
	SetConsoleColor(DEFAULT);
	va_end(argList);
}

void Util::Warn(LPWSTR string)
{
	va_list argList;
	va_start(argList, string);
	SetConsoleColor(LIGHT_YELLOW);
	wprintf(L"[-] ");
	vwprintf(string, argList);
	SetConsoleColor(DEFAULT);
	va_end(argList);
}

void Util::Error(DWORD error, LPWSTR string)
{
	SetConsoleColor(LIGHT_RED);
	wprintf(L"[!] %s: %s\n", string, NetErrorToString(error));
	SetConsoleColor(DEFAULT);
}

void Util::SetConsoleColor(WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Util::LsaUnicodeString(LPWSTR string, PLSA_UNICODE_STRING unicodeString)
{
	unicodeString->Buffer = string;
	unicodeString->Length = unicodeString->MaximumLength = lstrlen(string) * sizeof(WCHAR);
}

void Util::Print(LPWSTR s, ...)
{
	va_list argList;
	va_start(argList, s);
	SetConsoleColor(WHITE);
	vwprintf(s, argList);
	SetConsoleColor(DEFAULT);
	va_end(argList);
}

void Util::PrintLine()
{
	WORD i;
	SetConsoleColor(CYAN);
	for(i = 0; i < 80; i++) // 25 rows, 80 columns for the old console display
	{
		wprintf(L"-");
	}
	wprintf(L"\n");
	SetConsoleColor(DEFAULT);
}


// TODO: Make WideString and AsciiString more resilient to edge cases;
// at very minimum it should be bouncing back attempts to write more than 4095 bytes.
LPWSTR Util::WideString(std::string s)
{
	WCHAR wideString[4096];
	ZeroMemory(wideString, sizeof(wideString));
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wideString, s.length() * sizeof(WCHAR));
	return wideString;
}

LPWSTR Util::WideString(char *s)
{
	WCHAR wideString[4096];
	memset(wideString, 0, sizeof(wideString));
	MultiByteToWideChar(CP_UTF8, 0, s, -1, wideString, strlen(s) * sizeof(WCHAR));
	return wideString;
}

char *Util::AsciiString(LPWSTR s)
{
	char string[4096];
	ZeroMemory(string, sizeof(string));
	WideCharToMultiByte(CP_UTF8, 0, s, -1, string, lstrlen(s), NULL, NULL);
	return string;
}

LPWSTR Util::DomainFromIP(LPWSTR machine)
{
	PWKSTA_INFO_100 workInfo;
	std::string retString;
	DWORD ret;

	//ZeroMemory(retString, sizeof(retString));

	if((ret = NetWkstaGetInfo(machine, 100, (LPBYTE *)&workInfo)) != NERR_Success)
	{
		Util::Error(ret, L"NetWkstaGetInfo()");
		wprintf(L"Ret = %x\n", ret);
	}
	else
	{
		retString.append(Util::AsciiString(workInfo->wki100_langroup));
	}
	NetApiBufferFree(workInfo);
	//wprintf(L"Domain: %s\n", Util::WideString(retString.c_str()));

	// Well apparently my "useless" (according to me prior) conversion functions have
	// come in handy and fixed the aforementioned problem with returning a free'd string.
	return  Util::WideString(retString.c_str());
}

void Util::EnumShares()
{
	PSHARE_INFO_0 shareInfo;
	DWORD entriesRead;
	DWORD total;
	DWORD i;

	NetShareEnum(Config::machine, 0, (LPBYTE *)&shareInfo, MAX_PREFERRED_LENGTH, &entriesRead, &total, NULL);

	for(i = 0; i < entriesRead; i++)
	{
		wprintf(L"Share: %s\n", shareInfo[i].shi0_netname);
	}
}