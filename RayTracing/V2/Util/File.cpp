#include "File.hpp"
#include <fstream>
#include "Check.h"
namespace BlackWalnut
{

	bool Atoi(std::string str, int *ptr) {
		try {
			*ptr = std::stoi(std::string(str.begin(), str.end()));
		}
		catch (...) {
			return false;
		}
		return true;
	}

	bool Atof(std::string str, float *ptr) {
		try {
			*ptr = std::stof(std::string(str.begin(), str.end()));
		}
		catch (...) {
			return false;
		}
		return true;
	}

	bool Atod(std::string str, double *ptr) {
		try {
			*ptr = std::stod(std::string(str.begin(), str.end()));
		}
		catch (...) {
			return false;
		}
		return true;
	}


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

	std::vector<float> ReadFloatFile(const std::string &filename)
	{
		FILE *f = fopen(filename.c_str(), "r");
		if (f == nullptr) {
			CHECK(0);
			//Error("%s: unable to open file", filename);
			return{};
		}

		int c;
		bool inNumber = false;
		char curNumber[32];
		int curNumberPos = 0;
		int lineNumber = 1;
		std::vector<float> values;
		while ((c = getc(f)) != EOF) {
			if (c == '\n')
				++lineNumber;
			if (inNumber) {
				if (curNumberPos >= (int)sizeof(curNumber))
					/*LOG_FATAL("Overflowed buffer for parsing number in file: %s at "
						"line %d",
						filename, lineNumber);*/
					CHECK(0);
				// Note: this is not very robust, and would accept something
				// like 0.0.0.0eeee-+--2 as a valid number.
				if ((isdigit(c) != 0) || c == '.' || c == 'e' || c == 'E' || c == '-' ||
					c == '+') {
					CHECK_LT(curNumberPos, sizeof(curNumber));
					curNumber[curNumberPos++] = c;
				}
				else {
					curNumber[curNumberPos++] = '\0';
					float v;
					if (!Atof(curNumber, &v))
						/*ErrorExit("%s: unable to parse float value \"%s\"", filename,
							curNumber);*/
						CHECK(0);
					values.push_back(v);
					inNumber = false;
					curNumberPos = 0;
				}
			}
			else {
				if ((isdigit(c) != 0) || c == '.' || c == '-' || c == '+') {
					inNumber = true;
					curNumber[curNumberPos++] = c;
				}
				else if (c == '#') {
					while ((c = getc(f)) != '\n' && c != EOF)
						;
					++lineNumber;
				}
				else if (isspace(c) == 0) {
					/*Error("%s: unexpected character \"%c\" found at line %d.", filename, c,
						lineNumber);*/
					CHECK(0);
					return{};
				}
			}
		}
		fclose(f);
		return values;
	}

}


