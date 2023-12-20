#include "AnalisysHierarcy.h"
#include "framework.h"
#include "autoincludeHierarcy.h"
#include "ImagePathClass.h"
#pragma warning (disable: 4996)

void AnalisysHierarcy::ProcessFile(std::string& filePath)
{
    std::ifstream file(filePath);
    std::string line;

    std::string currentFile = fs::path(filePath).filename().string();

    while (std::getline(file, line))
    {
        size_t pos = line.find("#include");
        if (pos != std::string::npos)
        {
            std::string dependency = line.substr(pos + 9);
            dependency = dependency.substr(dependency.find_first_of("\"<") + 1);
            dependency = dependency.substr(0, dependency.find_last_of("\">"));

            fs::path dependencyPath = fs::path(filePath).parent_path() / dependency;
            if (fs::exists(dependencyPath))
            {
                dependencyMap[currentFile].insert(dependency);
            }
        }
    }
}

void AnalisysHierarcy::ProcessFilesInDirectory(const std::string& directoryPath)
{
    for (const auto& entry : fs::directory_iterator(directoryPath))
    {
        if (entry.is_regular_file())
        {
            std::string filePath = entry.path().string();
            if (filePath.size() >= 2 &&
                (filePath.compare(filePath.size() - 2, 2, ".h") == 0 || filePath.compare(filePath.size() - 4, 4, ".cpp") == 0 || filePath.compare(filePath.size() - 3, 3, ".rc") == 0))
            {
                ProcessFile(filePath);
            }
        }
        else if (entry.is_directory())
        {
            ProcessFilesInDirectory(entry.path().string());
        }
    }
}

void AnalisysHierarcy::PrintDependencyHierarcy(const std::string& mainFile, std::ofstream& outFile, int level = 0)
{
    const std::unordered_set<std::string>& dependencies = dependencyMap[mainFile];

    for (const std::string& dependency : dependencies)
    {
        outFile << std::string(level * 2, ' ') << "\"" << mainFile << "\" -> \"" << dependency << "\" [style=\"solid\", color=\"black\", shape=\"box\", dir=\"back\", label=\"#include\"]\n";

        PrintDependencyHierarcy(dependency, outFile, level + 1);
    }
}

void AnalisysHierarcy::WriteUmlToFile(const std::string& mainFile, std::string& outputFileName)
{
    std::ofstream outFile(outputFileName);
    outFile << "digraph UMLDiagram {\n";
    outFile << "node [shape=box];\n";

    PrintDependencyHierarcy(mainFile, outFile);

    outFile << "}\n";
    outFile.close();
}

void AnalisysHierarcy::ConvertDotToImage(std::string& dotFileName, std::string& outputImageName)
{
    std::string dotPath = FindDotExePath();

    if (!dotPath.empty())
    {
        STARTUPINFOA si = { sizeof(STARTUPINFO) };
        PROCESS_INFORMATION pi;

        std::string commandLine = dotPath + " -Tpng " + dotFileName + " -o " + outputImageName;
        if (CreateProcessA(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

std::string AnalisysHierarcy::ConvertWideTuChar(const wchar_t* wideString)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideString, -1, result.data(), size, nullptr, nullptr);
    return result;
}

std::string AnalisysHierarcy::FindDotExePath()
{
    char buffer[MAX_PATH];
    if (SearchPathA(NULL, "dot.exe", NULL, MAX_PATH, buffer, NULL) != 0)
    {
        return buffer;
    }
    return "";
}

void AnalisysHierarcy::SetProjectPath(wchar_t* path)
{
    std::wstring ws(path);
	projectPath = std::string(ws.begin(), ws.end());
}

void AnalisysHierarcy::SetMainFile(wchar_t* mainFile)
{
    std::wstring ws(mainFile);
    mainFileName = std::string(ws.begin(), ws.end());
}

void AnalisysHierarcy::StartBuilding()
{
	fs::path absolute = fs::absolute(projectPath);

    outputImagePath = absolute.string() + std::string("\\") + outputImageName;
    ImagePathClass imagePathClass;
    imagePathClass.SetImagePath(outputImageName);

    ProcessFilesInDirectory(absolute.string());
    WriteUmlToFile(mainFileName, dotFileName);
    ConvertDotToImage(dotFileName, outputImageName);
}
