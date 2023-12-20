#include "ImagePathClass.h"

std::string ImagePathClass::imagePath;

void ImagePathClass::SetImagePath(std::string& outputImagePath)
{
	imagePath = outputImagePath;
}

std::string ImagePathClass::GetImagePath()
{
	return imagePath;
}

