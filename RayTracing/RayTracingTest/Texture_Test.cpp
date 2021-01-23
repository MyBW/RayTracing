#include"gtest/gtest.h"
#include "../V2/Base/Textures.hpp"

using namespace BlackWalnut;
TEST(Texture, Blackbody)
{
	std::vector<std::string> FileNames = { "0.png","1.png" ,"2.png" ,"3.png" ,"4.png" ,"5.png" ,"6.png" ,"7.png" ,"8.png" ,"9.png" ,"10.png" };
	float MaxAniso = 8.0f;
	float Scale = 1.0;
	std::string FileName = "rock.png";
	std::string Filter = "bilinear";
	SpectrumImageTexture* TestTexture = new SpectrumImageTexture(new UVMapping2D(), FileName, Filter, MaxAniso, WrapMode::Repeat, Scale, ColorEncodingBase::sRGB);
	int Levels = TestTexture->mipmap->Levels();
	for (int i = 0; i < Levels; i++)
	{
		Image& Level = TestTexture->mipmap->GetLevelImage(i);
		ASSERT_TRUE(Level.Write(FileNames[i]));
	}
}