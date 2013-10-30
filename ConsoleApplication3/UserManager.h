#pragma once
#include "stdafx.h"

class UserManager
{
public:
	std::vector<User *> users;

	// Print user names only.
	BOOL EnumerateUserNames(LPCWSTR domain);
	// Print detailed user information per-user.
	void EnumerateUserInformation(LPCWSTR domain); 
	// Enumerate User SIDs; populate the vector with them.
	void AddUser(User *user);
	void DeleteUser(LPCWSTR accountName);
	void Clear();
	void DumpToCSV();
};