#include "stdafx.h"

void User::PrettyPrint()
{

	if(lstrlen(accountName.c_str()) > 0)
	{
		wprintf(L"Account name: %s\n", accountName.c_str()); 
	}

	wprintf(L"User role: ");

	switch(userPrivs)
	{
	case USER_PRIV_GUEST:
		wprintf(L"Guest\n");
		break;
	case USER_PRIV_USER:
		wprintf(L"User\n");
		break;
	case USER_PRIV_ADMIN:
		wprintf(L"Administrator\n");
		break;
	default:
		wprintf(L"Unknown\n");
	}

	if(lstrlen(fullName.c_str()) > 0)
	{
		wprintf(L"Full name: %s\n", fullName.c_str());
	}

	if(lstrlen(homePath.c_str()) > 0)
	{
		wprintf(L"Home path: %s\n", homePath.c_str());
	}

	if(lstrlen(scriptPath.c_str()) > 0)
	{
		wprintf(L"Script path: %s\n", scriptPath.c_str());
	}

	if(lstrlen(workStations.c_str()) > 0)
	{
		wprintf(L"Workstations: %s\n", workStations.c_str());
	}

	if(lastLogin > 0)
	{
		time_t loginTime = (time_t)lastLogin;
		tm timer;
		localtime_s(&timer, &loginTime); // This is apparently safer than localtime()? No idea why, but the compiler complains if you use it.
		wprintf(L"Last login: %d/%d/%d @ %02d:%02d:%02d\n", timer.tm_mon+1, timer.tm_mday, timer.tm_year+1900, timer.tm_hour, timer.tm_min, timer.tm_sec);
	}

	if(lastLogout > 0)
	{
		time_t logoutTime = (time_t)lastLogout;
		tm timer;
		localtime_s(&timer, &logoutTime); 
		wprintf(L"Last logout: %d/%d/%d @ %02d:%02d:%02d\n", timer.tm_mon+1, timer.tm_mday, timer.tm_year+1900, timer.tm_hour, timer.tm_min, timer.tm_sec);
	}
	
	if(loginCount != NULL && loginCount > 0)
	{
		wprintf(L"Total logins: %lu\n", loginCount);
	}

	if(lstrlen(userComment.c_str()) > 0)
	{
		wprintf(L"Comments: %s\n", userComment.c_str());
	}
	if(groups.size() > 0)
	{
		DWORD j;
		wprintf(L"Group size: %d\n", groups.size());
		wprintf(L"Groups: ");
		for(j = 0; j < groups.size(); j++)
		{
			if(j < groups.size()-1)
			{
				wprintf(L"%s, ", groups[j]->c_str());
			}
			else
			{
				wprintf(L"%s\n", groups[j]->c_str());
			}
		}
	}
	wprintf(L"\n");
	Util::PrintLine();
}