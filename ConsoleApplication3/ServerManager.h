#pragma once
#include "stdafx.h"

class ServerManager
{
public:
	std::vector<Server *> servers;

	void EnumerateServerInformation(LPWSTR domain);
	void DumpToCSV();
	void Clear();
};