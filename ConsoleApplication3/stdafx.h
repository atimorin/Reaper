// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
// Win32-specific includes
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <LM.h>
#include <time.h>
#include <Ntsecapi.h>
#include <Winnetwk.h>
#include <Sddl.h>
#include <Winldap.h>
#include <WinBer.h>
#include <IPHlpApi.h>
#include <Icmpapi.h>

// Added for memory leak detection
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

// C++ specific includes
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// Application-specific includes
#include "User.h"
#include "UserManager.h"
#include "Help.h"
#include "Session.h"
#include "Server.h"
#include "ServerManager.h"
#include "Util.h"
#include "LSA.h"
#include "Config.h"
#include "CSV.h"
#include "Group.h"
#include "GroupManager.h"
#include "AD.h"
// Definitions

// Color definitions for terminal attributes.
// Foreground colors.
#define BLACK 0x00
#define BLUE 0x01
#define GREEN 0x02
#define CYAN 0x03
#define RED 0x04
#define PURPLE 0x05
#define YELLOW 0x06
#define LIGHT_GREY 0x07
#define GREY 0x08
#define DARK_BLUE 0x09
#define LIGHT_GREEN 0x0A
#define LIGHT_BLUE 0x0B
#define LIGHT_RED 0x0C
#define MAGENTA 0x0D
#define LIGHT_YELLOW 0x0E
#define WHITE 0x0F

// Background colors.
#define BG_BLACK 0x00
#define BG_BLUE 0x10
#define BG_GREEN 0x20
#define BG_CYAN 0x30
#define BG_RED 0x40
#define BG_PURPLE 0x50
#define BG_YELLOW 0x60
#define BG_LIGHT_GREY 0x70
#define BG_GREY 0x80

// Default color schemes
#define DEFAULT (WHITE | BG_BLACK)
#define SYSTEM_DEFAULT (LIGHT_GREY | BG_BLACK)

// Definition for areas of heap memory that are allocated but not initialized.
// This is mainly for user info struct things like total number of logins,
// since apparently the function that populates the user info struct will not
// initialize fields to zero.
#define UNINITIALIZED 0xCDCDCDCD
#define REAPER_VERSION "0.01"