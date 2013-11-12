#pragma once

#include "stdafx.h"

class DNS
{
public:
	static bool IsIPv4(LPWSTR s);
	static void IPToHostname(LPWSTR ip, std::wstring *hostname);
};

