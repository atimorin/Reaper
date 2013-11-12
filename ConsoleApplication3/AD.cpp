#include "stdafx.h"

std::wstring AD::host = L"";
std::wstring AD::user = L"";
std::wstring AD::pass = L"";
std::wstring AD::dn   = L"";
std::wstring AD::filter = L"";

LDAP *AD::ldap = NULL;
SecPkgContext_CertInfo AD::sslInfo;
UserManager *AD::userManager = NULL;
GroupManager *AD::groupManager = NULL;
ServerManager *AD::serverManager = NULL;


void AD::SetHost(LPWSTR s)
{
	host = (wchar_t *)s;
}

LPWSTR AD::GetHost()
{
	return (LPWSTR)host.c_str();
}

void AD::SetUser(LPWSTR s)
{
	user = s;
}

void AD::SetPass(LPWSTR s)
{
	pass = s;
}

LPWSTR AD::GetUser()
{
	return (LPWSTR)user.c_str();
}

LPWSTR AD::GetPass()
{
	return (LPWSTR)pass.c_str();
}

void AD::SetUserManager(UserManager *u)
{
	userManager = u;
}

UserManager *AD::GetUserManager()
{
	return userManager;
}

void AD::SetGroupManager(GroupManager *g)
{
	groupManager = g;
}

GroupManager *AD::GetGroupManager()
{
	return groupManager;
}

void AD::SetServerManager(ServerManager *s)
{
	serverManager = s;
}

ServerManager *AD::GetServerManager()
{
	return serverManager;
}

bool AD::Connect()
{
	DWORD ret;
	LONG value;

	//if(!Init())
	//{
		if(!InitSSL())
		{
			Util::Warn(L"Unable to initialize an LDAP context.");
			return false;
		}
		else
		{
			if((ret = ldap_get_option(ldap, LDAP_OPT_SSL, (void *)&value)) != LDAP_SUCCESS)
			{
				Util::Error(LdapGetLastError(), L"ldap_get_option()");
			}
			if((void *)value == LDAP_OPT_ON)
			{
				if((ret = ldap_set_option(ldap, LDAP_OPT_SSL, LDAP_OPT_ON)) != LDAP_SUCCESS)
				{
					Util::Error(LdapGetLastError(), L"ldap_set_option()");
				}
			}
		}
	//}

	if(ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, (void *)LDAP_VERSION3) != LDAP_SUCCESS)
	{
		Util::Error(LdapGetLastError(), L"ldap_set_option()");
		wprintf(L"Ret = %x\n", LdapGetLastError());
	}
	return (ldap_connect(ldap, NULL) == LDAP_SUCCESS) ? true : false;
}


bool AD::Init()
{
	ldap = ldap_init((PWCHAR)host.c_str(), LDAP_PORT);
	
	return ldap == NULL ? false : true;
}

bool AD::InitSSL()
{
	std::wstring hostname;

	if(ldap != NULL)
	{
		ldap_unbind(ldap);
	}

	if(DNS::IsIPv4((PWCHAR)host.c_str()))
	{
		DNS::IPToHostname((PWCHAR)host.c_str(), &hostname);
		wprintf(L"hostname = %s\n", hostname.c_str());
		ldap = ldap_sslinit((PWCHAR)hostname.c_str(), LDAP_SSL_PORT, 1);
	}
	else
	{
		ldap = ldap_sslinit((PWCHAR)host.c_str(), LDAP_SSL_PORT, 1);
	}
	return ldap == NULL ? false : true;
}

bool AD::Login()
{
	if(!Connect())
	{
		Util::Warn(L"Couldn't connect via LDAP.\n");
		wprintf(L"ret = %x\n", LdapGetLastError());
		return false;
	}
	if(ldap_bind_s(ldap, NULL, NULL, LDAP_AUTH_NEGOTIATE) != LDAP_SUCCESS)
	{
		Util::Error(LdapGetLastError(), L"ldap_bind_s()");
		return false;
	}

	return true;
}

void AD::SetFilter(LPWSTR s)
{
	filter = s;
}

LPWSTR AD::GetFilter()
{
	return (LPWSTR)filter.c_str();
}

void AD::SetDN(LPWSTR s)
{
	dn = s;
}

void AD::GetDN()
{
	LPWSTR filters[2];
	LDAPMessage *search;
	LDAPMessage *entry;
	PWCHAR attribute;
	PWCHAR *value;
	BerElement *berElement;

	filters[0] = L"rootDomainNamingContext";
	filters[1] = NULL;

	ldap = ldap_init((PWCHAR)host.c_str(), LDAP_PORT);
	// Null binding; to do that we simply do not call ldap_bind.

	if(ldap_connect(ldap, NULL) != LDAP_SUCCESS)
	{
		Util::Error(LdapGetLastError(), L"ldap_connect()");
		wprintf(L"Ret = %x\n", LdapGetLastError());
	}
	if(ldap_search_s(ldap, L"", LDAP_SCOPE_BASE, L"(&(objectClass=*))", filters, 0, &search) != LDAP_SUCCESS)
	{
		Util::Error(LdapGetLastError(), L"ldap_search_s()");
		wprintf(L"Ret= %x\n", LdapGetLastError());
	}
	entry = ldap_first_entry(ldap, search);
	attribute = ldap_first_attribute(ldap, entry, &berElement);

	if(lstrcmpi(L"rootDomainNamingContext", attribute) == 0)
	{
		value = ldap_get_values(ldap, entry, attribute);
		wprintf(L"value = %s\n", value[0]);
		dn = value[0];
	}
	else
	{
		dn.clear();
	}
	ldap_value_free(value);
	ldap_memfree(attribute);
	ldap_msgfree(search);
	ber_free(berElement, 0);
}

void AD::Enumerate(ADSearchFilter searchFilter)
{
	LDAPMessage *search = NULL;
	LDAPMessage *entry = NULL;
	PWCHAR *value;
	PWCHAR attribute;
	berval *ber = NULL;
	DWORD i;
	DWORD j;
	// 20 strings should be enough?
	LPWSTR filters[20];
	BerElement *berElement = NULL;

	// Fill our filters array (NULL-terminated) with search-specific filters.
	switch(searchFilter)
	{
	case AD_SEARCH_USER:
		SetFilter(L"(&(objectClass=user)(objectCategory=person))");
		filters[0] = L"samAccountName";
		filters[1] = L"cn";
		filters[2] = L"homeDirectory";
		filters[3] = L"memberOf";
		filters[4] = NULL;
		break;
	case AD_SEARCH_GROUP:
		SetFilter(L"(&(objectCategory=group))");
		filters[0] = L"samAccountName";
		filters[1] = L"cn";
		filters[2] = L"member";
		filters[3] = NULL;
		break;
	case AD_SEARCH_MACHINE:
		SetFilter(L"(&(objectCategory=computer))");
		filters[0] = L"samAccountName";
		filters[1] = L"cn";
		filters[2] = NULL;
		break;
	default:
		break;
	}

	if(ldap_search_s(ldap, (const PWCHAR)dn.c_str(), LDAP_SCOPE_SUBTREE, (PWCHAR)filter.c_str(), NULL, 0, &search) != LDAP_SUCCESS)
	{
		Util::Error(LdapGetLastError(), L"ldap_search");
		Util::Notice(L"ret = %x\n", LdapGetLastError());
	}

	switch(searchFilter)
	{
	case AD_SEARCH_USER:
		AddUsers(search);
		break;
	case AD_SEARCH_GROUP:
		AddGroups(search);
		break;
	case AD_SEARCH_MACHINE:
		AddServers(search);
		break;
	default:
		break;
	}
	ldap_msgfree(search);
	ber_free(berElement, 0);
}

bool AD::Disconnect()
{
	return (ldap_unbind(ldap) == LDAP_SUCCESS);
}

void AD::AddUsers(LDAPMessage *search)
{
	DWORD i;
	DWORD j;
	LDAPMessage *entry = NULL;
	PWCHAR attribute;
	PWCHAR *values;
	BerElement *berElement;

	for(i = 0; i < ldap_count_entries(ldap, search); i++)
	{
		User *u = new User();
		if(!i)
		{
			entry = ldap_first_entry(ldap, search);
		}
		else
		{
			entry = ldap_next_entry(ldap, entry);
		}

		attribute = ldap_first_attribute(ldap, entry, &berElement);

		while(attribute != NULL)
		{
			//wprintf(L"%s: ", attribute);
			values = ldap_get_values(ldap, entry, attribute);

			u->sidString = L"N/A";
			u->lastLogin = 0;
			u->lastLogout = 0;
			if(lstrcmpi(attribute, L"samaccountname") == 0)
			{
				u->accountName.append(values[0]);
			}
			if(lstrcmpi(attribute, L"cn") == 0)
			{
				u->fullName.append(values[0]);
			}
			if(lstrcmpi(attribute, L"homedirectory") == 0)
			{
				u->homePath.append(values[0]);
			}
			if(lstrcmpi(attribute, L"memberof") == 0)
			{
				for(j = 0; j < ldap_count_values(values); j++)
				{
					std::wstring groupString = values[j];
					// The 3 offset from left makes sense (to cull out the CN=)
					// but I'm not quite sure whey I need to find()-3. Here's
					// hoping that doesn't break when tested out in the world.
					std::wstring subString = groupString.substr(3, groupString.find(L",")-3);
					std::wstring *testString = new std::wstring(subString);

					if(subString.length() > 0)
					{
						u->groups.push_back(new std::wstring(subString));
					}
				}
			}
			ldap_value_free(values);
			ldap_memfree(attribute);
			attribute = ldap_next_attribute(ldap, entry, berElement);
		}

		ber_free(berElement, 0);
		// Moment of truth; we only want accounts with account names.
		if(u->accountName.length() > 0)
		{
			GetUserManager()->users.push_back(u);
			u->PrettyPrint();
		}
		else
		{
			delete(u);
		}
	}
}

void AD::AddGroups(LDAPMessage *search)
{
	DWORD i;
	DWORD j;
	LDAPMessage *entry = NULL;
	PWCHAR attribute;
	PWCHAR *values;
	BerElement *berElement;

	for(i = 0; i < ldap_count_entries(ldap, search); i++)
	{
		Group *g = new Group();

		if(!i)
		{
			entry = ldap_first_entry(ldap, search);
		}
		else
		{
			entry = ldap_next_entry(ldap, entry);
		}

		attribute = ldap_first_attribute(ldap, entry, &berElement);
		
		while(attribute != NULL)
		{
			values = ldap_get_values(ldap, entry, attribute);

			if(lstrcmpi(attribute, L"samaccountname") == 0)
			{
				g->name = values[0];
			}
			if(lstrcmpi(attribute, L"member") == 0)
			{
				for(j = 0; j < ldap_count_values(values); j++)
				{
					std::wstring *ret = new std::wstring();
					CNToAccountName(values[j], ret);
					g->users.push_back(ret);
				}
			}
			ldap_value_free(values);
			ldap_memfree(attribute);
			attribute = ldap_next_attribute(ldap, entry, berElement);
		}

		ber_free(berElement, 0);

		if(g->name.length() > 0)
		{
			GetGroupManager()->groups.push_back(g);
			g->PrettyPrint();
		}
		else
		{
			delete(g);
		}
		
	}
}

void AD::AddServers(LDAPMessage *search)
{
	DWORD i;
	DWORD j;
	LDAPMessage *entry = NULL;
	BerElement *berElement = NULL;
	PWCHAR attribute;
	PWCHAR *values;

	for(i = 0; i < ldap_count_entries(ldap, search); i++)
	{
		Server *s = new Server();

		if(!i)
		{
			entry = ldap_first_entry(ldap, search);
		}
		else
		{
			entry = ldap_next_entry(ldap, entry);
		}

		attribute = ldap_first_attribute(ldap, entry, &berElement);

		while(attribute != NULL)
		{
			values = ldap_get_values(ldap, entry, attribute);

			if(lstrcmpi(attribute, L"samaccountname") == 0)
			{
				s->name = values[0];
				s->name.resize(s->name.length()-1);
			}
			ldap_value_free(values);
			ldap_memfree(attribute);
			attribute = ldap_next_attribute(ldap, entry, berElement);
		}

		if(s->name.length() > 0)
		{
			GetServerManager()->servers.push_back(s);
			s->PrettyPrint();
		}
		else
		{
			delete(s);
		}
	}
	ber_free(berElement, 0);
}

// Sorry that this is kind of crappy, but passing the point to wstring
// is basically necessary unless you want thrashed memory for strings.
void AD::CNToAccountName(LPWSTR s, std::wstring *d)
{
	LDAPMessage *search = NULL;
	LDAPMessage *entry = NULL;
	PWCHAR attribute;
	PWCHAR *values;
	BerElement *berElement = NULL;
	DWORD i;
	DWORD j;
	LPWSTR filters[2];

	filters[0] = L"samAccountName";
	filters[1] = NULL;

	d->clear();

	if(ldap_search_s(ldap, s, LDAP_SCOPE_SUBTREE, NULL, filters, 0, &search) != LDAP_SUCCESS)
	{
		Util::Error(LdapGetLastError(), L"ldap_search_s()");
	}

	entry = ldap_first_entry(ldap, search);

	for(i = 0; i < ldap_count_entries(ldap, search); i++)
	{
		if(!i)
		{
			entry = ldap_first_entry(ldap, search);
		}
		else
		{
			entry = ldap_next_entry(ldap, entry);
		}
		
		attribute = ldap_first_attribute(ldap, entry, &berElement);

		while(attribute != NULL)
		{
			values = ldap_get_values(ldap, entry, attribute);
			if(lstrcmpi(attribute, L"samaccountname") == 0)
			{
				d->append(values[0]);
			}
			ldap_value_free(values);
			ldap_memfree(attribute);
			attribute = ldap_next_attribute(ldap, entry, berElement);
		}
	}
	ber_free(berElement, 0);
	ldap_msgfree(search);
}