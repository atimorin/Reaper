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
	if(!Init())
	{
		if(!InitSSL())
		{
			Util::Warn(L"Unable to initialize an LDAP context.");
			return false;
		}
	}
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
	ldap = ldap_sslinit((PWCHAR)host.c_str(), LDAP_SSL_PORT, 1);

	return ldap == NULL ? false : true;
}

bool AD::Login()
{
	if(!Connect())
	{
		Util::Warn(L"Couldn't connect via LDAP.\n");
		return false;
	}
	if(ldap_bind_s(ldap, NULL, NULL, LDAP_AUTH_NTLM) != LDAP_SUCCESS)
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
	//if(ldap_bind_s(ldap, L"", NULL, LDAP_AUTH_NTLM) != LDAP_SUCCESS)
	//{
	//	Util::Error(LdapGetLastError(), L"ldap_bind()");
	//	wprintf(L"Ret = %x\n", LdapGetLastError());
	//}
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

void AD::EnumerateUsers()
{
	LDAPMessage *search = NULL;
	LDAPMessage *entry = NULL;
	LDAPControl *serverControl = NULL;
	PWCHAR matches = NULL;
	PWCHAR *value;
	PWCHAR attribute;
	berval *ber = NULL;
	DWORD i;
	DWORD j;
	LPWSTR filters[2];
	BerElement *berElement = NULL;

	SetFilter(L"(&(objectClass=*))");
	filters[0] = L"namingContexts";
	filters[1] = NULL;

	if(ldap_search_s(ldap, (const PWCHAR)dn.c_str(), LDAP_SCOPE_BASE, (const PWCHAR)filter.c_str(), NULL, 0, &search) != LDAP_SUCCESS)
	{
		Util::Error(LdapGetLastError(), L"ldap_search");
		Util::Notice(L"ret = %x\n", LdapGetLastError());
	}

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

		attribute = ldap_first_attribute(ldap, search, &berElement);
		wprintf(L"Entry #%d\n", i);
		while(attribute != NULL)
		{
			wprintf(L"\tAttribute: %s\n", attribute);
			value = ldap_get_values(ldap, entry, attribute);

			for(j = 0; j < ldap_count_values(value); j++)
			{
				wprintf(L"\tValue: %s\n", value[j]);
			}
			ldap_value_free(value);
			ldap_memfree(attribute);
			attribute = ldap_next_attribute(ldap, entry, berElement);
		}
	}
	ldap_msgfree(search);
	ber_free(berElement, 0);
}

bool AD::Disconnect()
{
	return (ldap_unbind(ldap) == LDAP_SUCCESS);
}