#include "File.hpp"
#include <fstream>
#include "Check.h"
namespace BlackWalnut
{

	int LastError() 
	{
		return 0;
		//return GetLastError();
//#ifdef PBRT_IS_WINDOWS
//		return GetLastError();
//#else
//		return errno;
//#endif
	}

	std::string ErrorString(int errorId /*= LastError()*/)
	{
		//char *s = NULL;
		//FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		//	FORMAT_MESSAGE_IGNORE_INSERTS,
		//	NULL, errorId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&s, 0,
		//	NULL);

		//std::string result = s;/* StringPrintf("%s (%d)", s, errorId);*/
		//LocalFree(s);

		//return result;
		return std::string();
	}

	std::string ReadFileContents(const std::string &filename)
	{
		std::ifstream ifs(filename, std::ios::binary);
		if (!ifs)
			//spdlog::error("[0]: [1]", filename, ErrorString());
			CHECK(0);
		return std::string((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
	}
}


