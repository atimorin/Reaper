#include "stdafx.h"

void Help::Usage(TCHAR *programName)
{
	Util::SetConsoleColor(CYAN);
	wprintf(L"\nUsage: %s [flags]\n\n", programName);
	wprintf(L"Flags:\n");
	wprintf(L"\t-t [target]\tSets target IP/UNC hostname to [target]\n");
	wprintf(L"\t-c [target]\tLogs output in CSV format to [target]\n");
	wprintf(L"\t-u [target]\tSets user name to [target]\n");
	wprintf(L"\t-p [target]\tSets password to [target]\n");
	wprintf(L"\t-l [target]\tSID enumeration for [user/group/machine]\n");
	wprintf(L"\t-b [target]\tNumber of SID brute force attempts.\n");
	wprintf(L"\t-n\t\tAuthenticate using SMB\n");
	wprintf(L"\t-i\t\tEnumerate account info over Net API\n");
	wprintf(L"\t-g\t\tEnumerate group info over Net API\n");
	wprintf(L"\t-m\t\tEnumerate machine info over Net API\n");
	wprintf(L"\n");
	Util::SetConsoleColor(SYSTEM_DEFAULT);
}