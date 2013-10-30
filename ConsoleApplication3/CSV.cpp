#include "stdafx.h"

// TODO: This CSV generation stuff is still pretty ghettofabulous, should 
// probably overhaul it to be less manual-string-packy-ish in design.
// Should probably also add special character detection/filtering/something
// to work around special character wonkiness in CSV.

// UPDATE: Well it's less of an abomination now.

void CSV::AddRow(LPWSTR string)
{
	std::wstring *newString = new std::wstring(string);
	rows.push_back(newString);
}

void CSV::Clear()
{
	DWORD i;
	for(i = 0; i < rows.size(); i++)
	{
		delete(rows[i]);
	}
	rows.clear();
}

void CSV::PrintToFile()
{
	DWORD i;
	FILE *file;

	if(_wfopen_s(&file, Config::csvFile, L"w") != 0)
	{
		Util::Warn(L"The CSV file could not be generated. Is it in use by another program?");
	}
	else
	{
		for(i = 0; i < rows.size(); i++)
		{
			fwprintf_s(file, L"%s\n", rows[i]->c_str());
		}
		fclose(file);
	}
}

void CSV::Print()
{
	DWORD i;
	wprintf(L"%d rows present.\n", rows.size());
	for(i = 0; i < rows.size(); i++)
	{
		wprintf(L"%s\n", rows[i]->c_str());
	}
}