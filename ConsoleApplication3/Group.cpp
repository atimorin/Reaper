#include "stdafx.h"

void Group::PrettyPrint()
{
	DWORD i;

	wprintf(L"Group name: %s\n", name.c_str());
	wprintf(L"Users: ");
	if(users.size() == 0)
	{
		wprintf(L"N/A\n");
	}
	for(i = 0; i < users.size(); i++)
	{
		if(i == users.size()-1)
		{
			wprintf(L"%s\n", users[i]->c_str());
		}
		else
		{
			wprintf(L"%s ", users[i]->c_str());
		}
	}
	wprintf(L"\n");
	Util::PrintLine();
}