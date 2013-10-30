#include "stdafx.h"

Config::Config()
{

}

// Init configs to default values.
WORD Config::versionMajor = 0;
WORD Config::versionMinor = 1;
bool Config::printUserNames = false;
bool Config::printUserInfo = false;
bool Config::printServerInfo = false;
bool Config::connectWNet = false;
bool Config::connectLSA = false;
LPWSTR Config::machine = L"";
LPWSTR Config::domain = L"";
bool Config::csvOutput = false;
LPWSTR Config::csvFile = L"";
bool Config::printGroupInfo = false;
// machineAccountHack used to tell the difference
// between machine and user account sorts.
bool Config::machineAccountHack = false;
SID_NAME_USE Config::sidType;
DWORD Config::numBruteTries = 9999;