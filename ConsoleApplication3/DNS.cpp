#include "stdafx.h"

// Should be robust enough to catch most edge cases.
bool DNS::IsIPv4(LPWSTR s)
{
	DWORD i;
	DWORD places = 0;
	DWORD octets = 0;
	LPWSTR parts[4];
	WCHAR *token = NULL;
	WCHAR *tmpString = new WCHAR[lstrlen(s)+1];
	WCHAR *context = NULL;

	ZeroMemory(tmpString, lstrlen(s)+1);

	for(i = 0; i < (DWORD)lstrlen(s); i++)
	{
		if((s[i] < 0x30 && s[i] != 0x2E) || (s[i] > 0x39 && s[i] != 0x2E))
		{
			return false;
		}
		if(s[i] >= 0x30 && s[i] <= 0x39)
		{
			places++;
		}
		if(s[i] == 0x2E)
		{
			octets++;
			places = 0;
		}
		if(places > 3 || octets > 4)
		{
			return false;
		}
	}

	lstrcpy(tmpString, s);

	token = wcstok_s(tmpString, L".", &context);

	while(token != NULL)
	{
		DWORD num = 0;

		num = _wtoi(token);

		if(num < 0 || num > 255)
		{
			return false;
		}

		token = wcstok_s(NULL, L".", &context);
	}
	delete(tmpString);
	return true;
}

void DNS::IPToHostname(LPWSTR ip, std::wstring *hostname)
{
	PDNS_RECORD dnsInfo;
	DWORD ret;
	WCHAR *token;
	WCHAR *context;
	std::wstring address;
	std::vector<LPWSTR> octets;

	LPWSTR tmpString = new WCHAR[lstrlen(ip)+1];
	ZeroMemory(tmpString, lstrlen(ip)+1);
	lstrcpy(tmpString, ip);

	hostname->clear();
	// Gotta flip the IP around for reverse DNS lookups.
	token = wcstok_s(tmpString, L".", &context);

	while(token != NULL)
	{
		octets.push_back(token);
		token = wcstok_s(NULL, L".", &context);
	}

	address.append(octets[3]);
	address.append(L".");
	address.append(octets[2]);
	address.append(L".");
	address.append(octets[1]);
	address.append(L".");
	address.append(octets[0]);
	address.append(L".in-addr.arpa");

	if((ret = DnsQuery((PWCHAR)address.c_str(), DNS_TYPE_PTR, DNS_QUERY_STANDARD, NULL, &dnsInfo, NULL)) != 0)
	{
		Util::Error(ret, L"DnsQuery()");
		wprintf(L"ret = %x\n", ret);
	}
	else
	{
		hostname->append(dnsInfo->Data.PTR.pNameHost);
		DnsRecordListFree(dnsInfo);
	}
	delete(tmpString);
}