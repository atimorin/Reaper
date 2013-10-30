#pragma once

#include "stdafx.h"

class Group
{
public:
	std::wstring name;
	std::vector<std::wstring *> users;
	
	Group()
	{
		name.clear();
	}

	void PrettyPrint();
};