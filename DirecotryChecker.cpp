#include "DirecotryChecker.h"
#include "commdlg.h"
#include <wchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


AnalisysHierarcy analisys;

void DirecotryChecker::ShowDirectoryFiles(HWND hWnd)
{
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        analisys.SetMainFile(ExtractMainFileFromDirectory(path));

        wchar_t* pathFolder = TrimToDirecotry(path);
        analisys.SetProjectPath(pathFolder);

        analisys.StartBuilding();
    }

    CoTaskMemFree(pidl);
}

wchar_t* DirecotryChecker::TrimToDirecotry(wchar_t* path)
{
    wchar_t* lastSlash = wcsrchr(path, L'\\');

    if (lastSlash != nullptr) {
        size_t newLength = static_cast<size_t>(lastSlash - path);

        wchar_t* newPath = new wchar_t[newLength + 1] {};

        wmemcpy(newPath, path, newLength);

        return newPath;
    }
    else {
        size_t length = wcslen(path);
        wchar_t* newPath = new wchar_t[length + 1] {};
        wcscpy_s(newPath, length + 1, path);
        
        return newPath;
    }
}

wchar_t* DirecotryChecker::ExtractMainFileFromDirectory(wchar_t* path)
{
    wchar_t* fileNamePtr = wcsrchr(path, L'\\');
    
    wchar_t* fileName = nullptr;
    
    if (fileNamePtr != nullptr) {
        fileName = _wcsdup(fileNamePtr + 1);
    }

    return fileName;
}
