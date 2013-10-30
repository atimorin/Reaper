#pragma once
#include "stdafx.h"

class Server
{
public:
	std::wstring name;
	DWORD type;
	std::vector<std::wstring *> shares;

	void PrettyPrint();
};