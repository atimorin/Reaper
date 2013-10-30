#include "stdafx.h"

void Server::PrettyPrint()
{
	DWORD i;

	wprintf(L"Server name: %s\n", name.c_str());
	wprintf(L"Shares: ");

	if(shares.size() == 0)
	{
		wprintf(L"N/A");
	}
	else
	{
		for(i = 0; i < shares.size(); i++)
		{
			if(i == shares.size()-1)
			{
				wprintf(L"%s\n", shares[i]->c_str());
			}
			else
			{
				wprintf(L"%s ", shares[i]->c_str());
			}
		}
	}
	wprintf(L"\n");
	Util::PrintLine();
}