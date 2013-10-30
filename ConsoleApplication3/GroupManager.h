#pragma once
#include "stdafx.h"

class GroupManager
{
public:
	std::vector<Group *> groups;
	
	void AddGroup();
	void EnumerateGroups(LPWSTR serverName);
	void DumpToCSV();
	void Clear();
};