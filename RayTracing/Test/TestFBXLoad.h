#pragma once
#include <string>
#include <vector>

struct DrawCallInfo 
{
	std::vector<float> BufferData;
	std::vector<unsigned int> IndexData;
	int VertexByteSize;
};
class TestFBXLoad
{
public:
	void ImportFBX(const std::string& FileName);
	DrawCallInfo drawInfo;
};
