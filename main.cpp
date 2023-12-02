#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <queue>

namespace fs = std::filesystem;

std::unordered_map<std::string, std::unordered_set<std::string>> dependencyMap; //storage for dependencies

void processFile(const std::string &filePath)
{
  std::ifstream file(filePath);
  std::string line;

  std::string currentFile = fs::path(filePath).filename().string();

  while (std::getline(file, line)) //checks file for '#include...'
  {    
    size_t pos = line.find("#include");
    if (pos != std::string::npos)
    {
      std::string dependency = line.substr(pos + 9); // '#include length = 8'

      // extract dependency file name
      dependency = dependency.substr(dependency.find_first_of("\"<") + 1);
      dependency = dependency.substr(0, dependency.find_last_of("\">"));
      
      // making a path to file 
      fs::path dependencyPath = fs::path(filePath).parent_path() / dependency;
      if (fs::exists(dependencyPath)) // if file exsists in directory, add it to dependency storage
      {
        dependencyMap[currentFile].insert(dependency);
      }
    }
  }
}

void processFilesInDirectory(const std::string &directoryPath) // finds .rc | .cpp | .h files and calling processFile for each
{
  for (const auto &entry : fs::directory_iterator(directoryPath))
  {
    if (entry.is_regular_file())
    {
      std::string filePath = entry.path().string();
      if (filePath.size() >= 2 &&
          (filePath.compare(filePath.size() - 2, 2, ".h") == 0 || filePath.compare(filePath.size() - 4, 4, ".cpp") == 0 || filePath.compare(filePath.size() - 3, 3, ".rc") == 0))
      {
        processFile(filePath);
      }
    }
    else if (entry.is_directory())
    {
      processFilesInDirectory(entry.path().string());
    }
  }
}

void printDependencyHierarchy(const std::string &mainFile, int level = 0)  //print hierary
{
  if (dependencyMap.find(mainFile) == dependencyMap.end()) //if file not found in storage
  {   
    return;
  }

  const std::unordered_set<std::string> &dependencies = dependencyMap[mainFile];

  for (const auto &dependency : dependencies)
  {
    for (int i = 0; i < level; ++i)
    {
      std::cout << "  "; // for spaces in hierarcy
    }
    std::cout << dependency << "\n";
    
    printDependencyHierarchy(dependency, level + 1);
  }
}

void processUnlinkedFiles(const std::string &projectPath)
{
  std::unordered_set<std::string> allFiles; // storage for all files in directory
  std::queue<std::string> unlinkedFiles; 

  for (const auto &entry : fs::directory_iterator(projectPath)) // add files in general storage 'allFiles'
  {
    if (entry.is_regular_file())
    {
      std::string filePath = entry.path().string();
      if (filePath.size() >= 2 &&
          (filePath.compare(filePath.size() - 2, 2, ".h") == 0 || filePath.compare(filePath.size() - 4, 4, ".cpp") == 0 || filePath.compare(filePath.size() - 3, 3, ".rc") == 0))
      {
        allFiles.insert(fs::path(filePath).filename().string());
      }
    }
  }

  for (const auto &[file, dependencies] : dependencyMap) //deletes file from storage if it included somewhere
  {
    allFiles.erase(file);
    for (const auto &dependency : dependencies)
    {
      allFiles.erase(dependency);
    }
  }

  for (const auto &unlinkedFile : allFiles) // add all files that stayed in 'allFiles'
  {
    unlinkedFiles.push(unlinkedFile);
  }

  std::cout << "Отдельные файлы, которые не включены нигде:\n";
  while (!unlinkedFiles.empty())
  {
    std::cout << unlinkedFiles.front() << "\n";
    unlinkedFiles.pop();
  }
}

int main()
{
  std::string projectPath;
  std::cout << "Введите путь к папке проекта: ";
  std::getline(std::cin, projectPath);

  std::string mainFile;
  std::cout << "Введите имя главного файла (с расширением, например, main.cpp): ";
  std::getline(std::cin, mainFile);

  // Преобразование относительного пути в абсолютный
  fs::path absolutePath = fs::absolute(projectPath);

  processFilesInDirectory(absolutePath.string());
  printDependencyHierarchy(mainFile);

  processUnlinkedFiles(absolutePath.string());

  return 0;
}