#pragma once
#include<string>
#include <vector>
namespace BlackWalnut
{
	int LastError();
	std::string ErrorString(int errorId = LastError());
	std::string ReadFileContents(const std::string &filename);
	std::vector<float> ReadFloatFile(const std::string &filename);
}