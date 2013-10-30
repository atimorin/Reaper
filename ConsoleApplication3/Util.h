#pragma once
#include "stdafx.h"

class Util
{
public:
	static WCHAR *ServerFlagsToString(DWORD flags);
	static WCHAR *NetErrorToString(DWORD error);
	static void Notice(LPWSTR string, ...);
	static void Warn(LPWSTR string);
	static void Error(DWORD error, LPWSTR string);
	static void SetConsoleColor(WORD color);
	static void LsaUnicodeString(LPWSTR string, PLSA_UNICODE_STRING unicodeString);
	static void Print(LPWSTR s, ...);
	static void PrintLine();
	static LPWSTR WideString(std::string s);
	static LPWSTR WideString(char *s);
	static char *AsciiString(LPWSTR);
	static LPWSTR DomainFromIP(LPWSTR machine);
	static void EnumShares();
};