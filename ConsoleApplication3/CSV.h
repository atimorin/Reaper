#pragma once
#include "stdafx.h"

class CSV 
{
private:
	std::vector<std::wstring *> rows;
	WORD rowCount;
	WORD columnCount;
public:
	CSV()
	{
		rowCount = 0;
		columnCount = 0;
	}

	void AddRow(LPWSTR string);
	void PrintToFile();
	void Clear();
	void Print();
};