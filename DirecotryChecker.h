#pragma once
#include "framework.h"
#include "AnalisysHierarcy.h"

class DirecotryChecker
{
public:
	void ShowDirectoryFiles(HWND);
private:
	LPITEMIDLIST pidl;
	wchar_t path[MAX_PATH];

	wchar_t* TrimToDirecotry(wchar_t* path);
	wchar_t* ExtractMainFileFromDirectory(wchar_t* path);
};

