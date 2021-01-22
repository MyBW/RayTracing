#pragma once
#include<string>
namespace BlackWalnut
{
	int LastError();
	std::string ErrorString(int errorId = LastError());
	std::string ReadFileContents(const std::string &filename);
}