#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{	
	// Initial manager setup
	UserManager userManager;
	ServerManager serverManager;
	GroupManager groupManager;
	Session session;
	session.userName = NULL;
	session.password = NULL;

	// Plumbing.
	LSA::SetUserManager(&userManager);
	LSA::SetGroupManager(&groupManager);
	LSA::SetServerManager(&serverManager);

	AD::SetUserManager(&userManager);
	AD::SetGroupManager(&groupManager);
	AD::SetServerManager(&serverManager);

	// So apparently we're a bunch of cavemen/cavewomen living in the stone age.
	// As such we have to roll our own UNIX-style command line parsing.
	// This is guaranteed to get ugly fast.
	// TODO: Create an options parsing static class to take care of this for us.

	DWORD i;
	for(i = 1; i < argc; i++) 
	{
		if(argv[i][0] == '-') 
		{
			TCHAR *option = argv[i]+1; 
			// Found a flag that needs an argument.
			if(lstrcmp(option, L"d") == 0) 
			{
				// Is the next argument empty or another flag? If so ya dun goofed.
				if(i+1 >= argc || argv[i+1][0] == '-') 
				{
					Help::Usage(argv[0]);
					exit(1);
				}
				else
				{
					Config::domain = argv[i+1];
				}
			}
			if(lstrcmp(option, L"u") == 0) 
			{
				if(i+1 >= argc || argv[i+1][0] == '-') 
				{
					Help::Usage(argv[0]);
					exit(1);
				}
				else
				{
					session.userName = argv[i+1];
				}
			}
			if(lstrcmp(option, L"p") == 0) 
			{
				if(i+1 >= argc || argv[i+1][0] == '-') 
				{
					Help::Usage(argv[0]);
					exit(1);
				}
				else
				{
					session.password = argv[i+1];
				}
			}
			if(lstrcmp(option, L"t") == 0) 
			{
				if(i+1 >= argc || argv[i+1][0] == '-') 
				{
					Help::Usage(argv[0]);
					exit(1);
				}
				else
				{
					Config::machine = argv[i+1];
				}
			}
			if(lstrcmp(option, L"c") == 0) 
			{
				if(i+1 >= argc || argv[i+1][0] == '-') 
				{
					Help::Usage(argv[0]);
					exit(1);
				}
				else
				{
					Config::csvOutput = true;
					Config::csvFile = argv[i+1];
				}
			}
			if(lstrcmp(option, L"b") == 0) 
			{
				if(i+1 >= argc || argv[i+1][0] == '-') 
				{
					Help::Usage(argv[0]);
					exit(1);
				}
				else
				{
					Config::numBruteTries = _wtoi(argv[i+1]);
				}
			}
			if(lstrcmp(option, L"l") == 0)
			{
				if(i+1 >= argc || argv[i+1][0] == '-')
				{
					Help::Usage(argv[0]);
					exit(1);
				}
				else
				{
					Config::connectLSA = true;

					if(lstrcmpi(argv[i+1], L"group") == 0)
					{
						Config::sidType = SidTypeGroup;
					}
					else if(lstrcmpi(argv[i+1], L"user") == 0)
					{
						Config::sidType = SidTypeUser;
						Config::machineAccountHack = false;
					}
					else if(lstrcmpi(argv[i+1], L"machine") == 0)
					{
						// As far as I can tell enumerable machine accounts
						// through LSA are just regular accounts marked with
						// a dollar sign at the end, so we use SidTypeUser
						// instead of SidTypeComputer, which returns nothing.
						Config::sidType = SidTypeUser;
						// TODO: Fix LSA enumeration so that we don't need this flag.
						Config::machineAccountHack = true;
					}
				}
			}
			if(lstrcmp(option, L"n") == 0)
			{
				Config::connectWNet = true;
			}
			if(lstrcmp(option, L"i") == 0)
			{
				Config::printUserInfo = true;
			}
			if(lstrcmp(option, L"s") == 0)
			{
				Config::printServerInfo = true;
			}
			if(lstrcmp(option, L"g") == 0)
			{
				Config::printGroupInfo = true;
			}
		}
	} 
	// End options parsing.
	//AD::SetHost(L"");
	//AD::GetDN();
	//AD::Login();
	//AD::Enumerate(AD_SEARCH_USER);

	// Console setup
	Util::SetConsoleColor(DEFAULT);

	// Check to make sure run-once conditions have been satisfied.
	if(!Config::printUserNames && !Config::printUserInfo && !Config::printServerInfo && !Config::connectWNet && !Config::connectLSA && !Config::printGroupInfo)
	{
		Help::Usage(argv[0]);
		exit(1);
	}
	//Cosmetic new-line; just makes the output look better.

	wprintf(L"\n");

	// Begin fun non-options-parsing stuff here.
	// Make sure to connect session first if needed.

	if(Config::connectWNet)
	{
		if(!session.userName || !session.password)
		{
			Util::Warn(L"Full NetBIOS credentials unspecified, skipping NetBIOS login.\n");
		}
		else if(lstrlen(Config::machine) <= 0)
		{
			Util::Warn(L"No target IP specified, skipping NetBIOS login.\n");
		}
		else
		{
			Util::Notice(L"Connecting via SMB...\n\n");
			session.ConnectWNet(session.userName, session.password, Config::machine);
		}
	}

	if(Config::connectLSA)
	{
		if(lstrlen(Config::machine) <= 0)
		{
			Util::Warn(L"No target IP specified, skipping LSA SID brute force.\n");
		}
		else if(!Config::sidType)
		{
			Util::Warn(L"Incorrect or no SID type specified, skipping LSA SID brute force.\n");
		}
		else
		{
			Util::Notice(L"Connecting via LSA...\n\n");
			LSA::OpenPolicy(&session, Config::machine);
			LSA::EnumerateSIDs(&session, Config::sidType);

			if(Config::csvOutput && lstrlen(Config::csvFile) > 0)
			{
				switch(Config::sidType)
				{
				case SidTypeUser:
					if(!Config::machineAccountHack)
					{
						userManager.DumpToCSV();
					}
					else
					{
						serverManager.DumpToCSV();
					}
					break;
				case SidTypeGroup:
					groupManager.DumpToCSV();
				default:
					break;
				}
			}
		}
	}

	if(Config::printUserInfo)
	{
		// We send the machine instead of the domain on purpose,
		// since it's faster than sending the name of the domain
		// and the domain name can be a faulty method from time to time.

		if(lstrlen(Config::machine) <= 0)
		{
			Util::Warn(L"No target IP specified, skipping NetBIOS user enumeration.\n");
		}
		else
		{
			Util::Notice(L"Printing user info now\n\n");
			userManager.EnumerateUserInformation(Config::machine); 

			if(Config::csvOutput && lstrlen(Config::csvFile) > 0)
			{
				userManager.DumpToCSV();
			}
		}
	}

	if(Config::printGroupInfo)
	{
		// Fairly sure we do the same as with the user information;
		// which is to say we send directly to the DC instead of using a
		// domain name.

		if(lstrlen(Config::machine) <= 0)
		{
			Util::Warn(L"No Target IP specified, skipping NetBIOS group enumeration.\n");
		}
		else
		{
			Util::Notice(L"Printing group info now\n");
			groupManager.EnumerateGroups(Config::machine);
			if(Config::csvOutput && lstrlen(Config::csvFile) > 0)
			{
				groupManager.DumpToCSV();
			}
		}
	}

	if(Config::printServerInfo)
	{
		// Once again pointing directly at the DC appears to
		// be superior to the option of supplying a domain name.
		if(lstrlen(Config::machine) <= 0)
		{
			Util::Warn(L"No target IP specified, skipping NetBIOS server enumeration.\n");
		}
		else
		{
			Util::Notice(L"Printing server info now...\n\n");
			serverManager.EnumerateServerInformation(Config::machine);
			if(Config::csvOutput && lstrlen(Config::csvFile) > 0)
			{
				serverManager.DumpToCSV();
			}
		}
	}

	if(Config::connectWNet && lstrlen(Config::machine) > 0)
	{
		session.DisconnectWNet(Config::machine);
	}

	Util::SetConsoleColor(SYSTEM_DEFAULT);

	// Be free memory. Be free as a bird.
	userManager.Clear();
	groupManager.Clear();
	serverManager.Clear();
	return 0;
}

