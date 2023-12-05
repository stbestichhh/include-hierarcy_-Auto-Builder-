#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <queue>
#include <windows.h>

namespace fs = std::filesystem;

std::unordered_map<std::string, std::unordered_set<std::string>> dependencyMap;

void processFile(const std::string &filePath)
{
  std::ifstream file(filePath);
  std::string line;

  std::string currentFile = fs::path(filePath).filename().string();

  while (std::getline(file, line))
  {
    // Простой пример: предполагаем, что зависимости указываются после ключевого слова '#include'
    size_t pos = line.find("#include");
    if (pos != std::string::npos)
    {
      std::string dependency = line.substr(pos + 9); // Длина "#include" равна 8
      dependency = dependency.substr(dependency.find_first_of("\"<") + 1);
      dependency = dependency.substr(0, dependency.find_last_of("\">"));

      // Добавляем зависимость только если файл существует в папке проекта
      fs::path dependencyPath = fs::path(filePath).parent_path() / dependency;
      if (fs::exists(dependencyPath))
      {
        dependencyMap[currentFile].insert(dependency);
      }
    }
  }
}

void processFilesInDirectory(const std::string &directoryPath)
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

void printDependencyHierarchy(const std::string &mainFile, std::ofstream &outFile, int level = 0)
{
  if (dependencyMap.find(mainFile) == dependencyMap.end())
  {
    std::cerr << "Главный файл не найден в зависимостях.\n";
    return;
  }

  const std::unordered_set<std::string> &dependencies = dependencyMap[mainFile];

  for (const auto &dependency : dependencies)
  {
    outFile << std::string(level * 2, ' ') << "\"" << mainFile << "\" -> \"" << dependency << "\" [style=\"solid\", color=\"black\", shape=\"box\", dir=\"back\", label=\"#include\"]\n";

    // Обработка зависимостей рекурсивно
    printDependencyHierarchy(dependency, outFile, level + 1);
  }
}

void writeUmlToFile(const std::string &mainFile, const std::string &outputFileName)
{
  std::ofstream outFile(outputFileName);
  outFile << "digraph UMLDiagram {\n";
  outFile << "node [shape=box];\n"; // Задаем форму узлов как квадраты

  printDependencyHierarchy(mainFile, outFile);

  outFile << "}\n";
  outFile.close();

  std::cout << "Иерархия зависимостей записана в файл " << outputFileName << std::endl;
}

std::string findDotExePath()
{
  char buffer[MAX_PATH];
  if (SearchPathA(NULL, "dot.exe", NULL, MAX_PATH, buffer, NULL) != 0)
  {
    return buffer;
  }
  return ""; // Возвращаем пустую строку в случае ошибки
}

void convertDotToImage(const std::string &dotFileName, const std::string &outputImageName)
{
  std::string dotPath = findDotExePath();

  if (!dotPath.empty())
  {
    // Создаем процесс dot.exe
    STARTUPINFOA si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;

    std::string commandLine = dotPath + " -Tpng " + dotFileName + " -o " + outputImageName;
    if (CreateProcessA(NULL, const_cast<char *>(commandLine.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
      // Ждем завершения процесса
      WaitForSingleObject(pi.hProcess, INFINITE);

      // Закрываем дескрипторы процесса и потока
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
    }
    else
    {
      std::cerr << "Ошибка при запуске dot.exe: " << GetLastError() << std::endl;
    }
  }
  else
  {
    std::cerr << "dot.exe не найден.\n";
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

  std::string dotFileName = "output.dot";
  std::string outputImageName = "output.png";

  // Преобразование относительного пути в абсолютный
  fs::path absolutePath = fs::absolute(projectPath);

  processFilesInDirectory(absolutePath.string());
  writeUmlToFile(mainFile, dotFileName);
  convertDotToImage(dotFileName, outputImageName);

  return 0;
}
