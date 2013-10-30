#pragma once
#include "stdafx.h"

// Global configs
class Config
{

public:

	static WORD versionMajor;
	static WORD versionMinor;
	static bool printUserNames;
	static bool printUserInfo;
	static bool printServerInfo;
	static bool connectWNet;
	static bool connectLSA;
	static LPWSTR machine;
	static LPWSTR domain;
	static bool csvOutput;
	static LPWSTR csvFile;
	static bool printGroupInfo;
	static SID_NAME_USE sidType;
	static DWORD numBruteTries;
	// Temporary fix for crappy sidtype stuff.
	static bool machineAccountHack;
	Config();
};