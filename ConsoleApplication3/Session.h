#pragma once
#include "stdafx.h"

class Session
{
public:
	LPWSTR userName;
	LPWSTR domain;
	LPWSTR password;
	HANDLE lsa;
	HANDLE token;
	LUID luid;

	void ConnectWNet(LPWSTR user, LPWSTR pass, LPWSTR host);
	void DisconnectWNet(LPWSTR host);
	void ConnectLSA(LPWSTR user, LPWSTR pass, LPWSTR host);
	void DisconnectLSA();
	void ConnectLDAP();
	void DisconnectLDAP();
};