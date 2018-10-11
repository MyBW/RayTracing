#pragma once

#include <iostream>

#include "GLAUX.H"
#include "glut.h"
#include <stdio.h>
#include <fstream>

AUX_RGBImageRec * CreateTextureFromBmp(char *FileName)
{
	FILE *File = NULL;
	File = fopen(FileName, "r");
	if (!File)
		return 0;
	fclose(File);
	return auxDIBImageLoad(FileName);
}

GLuint CreateTextrue(char *FileName)
{
	AUX_RGBImageRec * imageRec = CreateTextureFromBmp(FileName);
	if (!imageRec)
		exit(EXIT_FAILURE);
	GLuint texName;
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageRec->sizeX, imageRec->sizeY,
		0, GL_RGB, GL_UNSIGNED_BYTE, imageRec->data);
	return texName;
}

void GetLightMapData(const char *LightMapFileUVFileName, std::vector<BWPoint2DD> &LightmapUV)
{
	std::ifstream fin(LightMapFileUVFileName, std::ios::in | std::ios::binary | std::ios::ate);
	//fin.open(fileName);

	if (!fin.is_open())
		return;
	fin.seekg(0, std::ios::end);
	std::streamoff iCopyByte = fin.tellg();
	//iCopyByte--;
	fin.seekg(0, std::ios::beg);

	int iCopyBatch = 0;
	char* buffer = new char[1024 * 1024];
	char* FinalBuf = new char[iCopyByte];
	fin.read(FinalBuf, iCopyByte);
	long int FileSize = iCopyByte;
	//while (iCopyByte != 0)
	//	//while (!fin.eof())
	//{
	//	iCopyBatch = (int)iCopyByte > (1024 * 1024) ? 1024 * 1024 : iCopyByte;
	//	fin.read(buffer, iCopyBatch); //从文件中读取
	//	glodon_Write((const unsigned char*)buffer, iCopyBatch);//写入流中
	//	
	//														   //m_nSize += iCopyBatch;
	//														   //static int i = 0;
	//														   //std::cout << ++i << std::endl;
	//	iCopyByte -= iCopyBatch;
	//}
	delete[] buffer;

	fin.close();

	long int U16Num = FileSize / sizeof(unsigned short int);
	for (long int i = 0 ; i < U16Num; i+=2)
	{
		unsigned short int U = *((unsigned short int*)(FinalBuf + sizeof(unsigned short int) * i));
		unsigned short int V = *((unsigned short int*)(FinalBuf + sizeof(unsigned short int) * (i + 1)));
		LightmapUV.push_back(BWPoint2DD(U / 65535.0f, V / 65535.0f));
	}
}