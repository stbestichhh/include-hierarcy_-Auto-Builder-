#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <queue>
#include <codecvt>

namespace fs = std::filesystem;

class AnalisysHierarcy
{
private:
	std::unordered_map<std::string, std::unordered_set<std::string>> dependencyMap;

	std::string projectPath;
	std::string mainFileName = "Main.cpp";

	std::string dotFileName = "output.dot";
	std::string outputImageName = "output.png";

	std::string outputImagePath;

	void ProcessFile(std::string &filePath);
	void ProcessFilesInDirectory(const std::string &directoryPath);
	void PrintDependencyHierarcy(const std::string &mainFile, std::ofstream &outFile, int level);
	void WriteUmlToFile(const std::string &mainFile, std::string &outputFileName);
	void ConvertDotToImage(std::string &dotFileName, std::string &outputImageName);
	std::string ConvertWideTuChar(const wchar_t* wideString);
	std::string FindDotExePath();

public:
	void SetProjectPath(wchar_t* path);
	void SetMainFile(wchar_t* mainFile);
	
	void StartBuilding();
};

