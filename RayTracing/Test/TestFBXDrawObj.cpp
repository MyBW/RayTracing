//#include "TestFBXDrawObj.h"
//#include <vector>
//#include "..\Math\BWPrimitive.h"
//#include "..\RealTimeRenderer\RealTimeRenderer.h"
//#include <iostream>
//
//#include "GLAUX.H"
//#include "glut.h"
//#include <stdio.h>
//
//AUX_RGBImageRec * CreateTextureFromBmp()
//{
//	FILE *File = NULL;
//	const char *Filename = "lightmap-rgbm0.bmp";
//	File = fopen(Filename, "r");
//	if (!File)
//		return 0;
//	fclose(File);
//	return auxDIBImageLoad(Filename);
//
//}
//
//void TestFBXDrawObj::InitObj(RealTimeRenderer *RTRenderer, TestFBXLoad *Loader)
//{
//	DrawCallInfo &DCinfo = Loader->drawInfo;
//	IndexNum = DCinfo.IndexData.size();
//
//
//	int uvSize = DCinfo.IndexData.size();
//	glodon_MemoryStream memeoryStream;
//	memeoryStream.glodon_readFile(std::string("uvs1-export.buf"));
//	const unsigned char *uvBuf = memeoryStream.glodon_GetBuffer();
//	int BufSize = memeoryStream.glodon_GetBufSize();
//	int curBufPosByByte = 0;
//	std::vector<float> LightMapUV;
//	if (uvSize * 2 * 2 + 0 > BufSize) // 在lightmapuv.buf 中存放的是以16bit的整型数字，所以 u v 分别占用两个字节（uvSize * 2 * 2）
//	{
//		return;
//	}
//	for (int i = 0; i < uvSize; i++)
//	{
//		uint16 uU = *((uint16*)(uvBuf + curBufPosByByte)); curBufPosByByte += 2;
//		uint16 uV = *((uint16*)(uvBuf + curBufPosByByte)); curBufPosByByte += 2;
//		LightMapUV.push_back(uU / 65535.0f);
//		LightMapUV.push_back(uV / 65535.0f);
//	}
//
//	InitTexture();
//	
//	//float *vertex = new float[DCinfo.BufferData.size() / 13 * 4];
//	//for (int i = 0 , j  = 0; i < DCinfo.BufferData.size(); i+=13 , j+= 4)
//	//{
//	//	//std::cout << DCinfo.BufferData[i] 
//	//	//		 <<"  "<< DCinfo.BufferData[i + 1] 
//	//	//         <<"  " << DCinfo.BufferData[i + 2] 
//	//	//         <<"  " << DCinfo.BufferData[i + 3] << std::endl;
//	//	vertex[j] = DCinfo.BufferData[i];
//	//	vertex[j + 1] = DCinfo.BufferData[i + 1];
//	//	vertex[j + 2] = DCinfo.BufferData[i + 2];
//	//	vertex[j + 3] = DCinfo.BufferData[i + 3];
//	//	std::cout << vertex[j]
//	//		<< "  " << vertex[j + 1]
//	//		<< "  " << vertex[j + 2]
//	//		<< "  " << vertex[j + 3] << std::endl;
//	//}
//	glGenBuffers(1, &VertexBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * DCinfo.BufferData.size(), DCinfo.BufferData.data(), GL_STATIC_DRAW);
//
//	glGenBuffers(1, &LightmapBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, LightmapBuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * LightMapUV.size(), LightMapUV.data(), GL_STATIC_DRAW);
//
//
//	glGenBuffers(1, &IndexBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, IndexBuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * DCinfo.IndexData.size(), DCinfo.IndexData.data(), GL_STATIC_DRAW);
//
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
//	glEnableVertexAttribArray(RTRenderer->PosLoc);
//	glVertexAttribPointer(RTRenderer->PosLoc, 4, GL_FLOAT, GL_FALSE, DCinfo.VertexByteSize, NULL);
//
//	glBindBuffer(GL_ARRAY_BUFFER, LightmapBuffer);
//	glEnableVertexAttribArray(RTRenderer->UVLoc);
//	glVertexAttribPointer(RTRenderer->UVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
//	
//	//glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
//	//glEnableVertexAttribArray(RTRenderer->UVLoc);
//	//glVertexAttribPointer(RTRenderer->UVLoc, 4, GL_FLOAT, GL_FALSE, DCinfo.VertexByteSize, (GLvoid*)8);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
//
//	this->RTRenderer = RTRenderer;
//
//}
//
//void TestFBXDrawObj::InitTexture()
//{
//	AUX_RGBImageRec * imageRec = CreateTextureFromBmp();
//	if (!imageRec)
//		exit(EXIT_FAILURE);
//	
//	glGenTextures(1, &texName);
//	glBindTexture(GL_TEXTURE_2D, texName);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageRec->sizeX, imageRec->sizeY,
//		0, GL_RGB, GL_UNSIGNED_BYTE, imageRec->data);
//
//}
//
//void TestFBXDrawObj::Draw()
//{
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texName);
//	glUniform1i(RTRenderer->TextureLoc, 0);
//
//	RTRenderer->SetModelMatrix(BWMatrix4::IDENTITY);
//	glBindVertexArray(VAO);
//
//	glDrawElements(GL_TRIANGLES, IndexNum, GL_UNSIGNED_INT, NULL);
//}