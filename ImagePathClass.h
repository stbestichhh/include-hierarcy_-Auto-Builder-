#pragma once
#include <string>
static class ImagePathClass
{
private:
	static std::string imagePath;

public:
	void SetImagePath(std::string &outputImagePath);
	std::string GetImagePath();
};

