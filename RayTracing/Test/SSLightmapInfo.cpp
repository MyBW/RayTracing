
#include "SSLightmapInfo.h"
#include <fstream>
#include <iostream>

glodon_MemoryStream::glodon_MemoryStream(int nSize)
{
	m_nSize = nSize;
	m_nCapacity = nSize;
	m_nPos = 0;


	if (nSize <= 0)
	{
		m_nCapacity = INCREASE_MEMORY_SIZE;
	}
	try
	{
		m_pBuffer = new unsigned char[m_nCapacity];
		memset(m_pBuffer, 0, m_nCapacity * sizeof(unsigned char));
	}
	catch (...)
	{
		// new失败，按照c++标准，将抛出std::alloc异常
		m_pBuffer = NULL;
		m_nCapacity = 0;
		m_nSize = 0;
		m_nPos = 0;
	}

}

int glodon_MemoryStream::glodon_writeFile(const std::string & fileName)
{
	if (m_nSize == 0)
		return -1;

	//以二进制方式打开文件
	std::ofstream fout(fileName.c_str(), std::ios::trunc | std::ios::out | std::ios::binary);
	//fout.open(fileName.c_str(), std::ios::_Noreplace | std::ios::out | std::ios::binary); //打开文件
	//fout.open(fileName);

	if (!fout.is_open())
		return -2;
	//this->Reset();
	glodon_SetPos(0);
	int iCopyByte = m_nSize;
	int iCopyBatch = 0;
	while (iCopyByte != 0)
	{
		iCopyBatch = iCopyByte > 1024 ? 1024 : iCopyByte; 
		fout.write((const char*)m_pBuffer + m_nPos, iCopyBatch);
		m_nPos += iCopyBatch;
		iCopyByte -= iCopyBatch;
	}

	fout.close();

	return iCopyByte;
}

int glodon_MemoryStream::glodon_readFile(const std::string & fileName)
{
	std::ifstream fin(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	//fin.open(fileName);

	if (!fin.is_open())
		return -2;
	fin.seekg(0, std::ios::end);
	std::streamoff iCopyByte = fin.tellg();
	//iCopyByte--;
	fin.seekg(0, std::ios::beg);

	int iCopyBatch = 0;
	char* buffer = new char[1024 * 1024];
	while (iCopyByte != 0)
		//while (!fin.eof())
	{
		iCopyBatch = iCopyByte > 1024 * 1024 ? 1024 * 1024 : iCopyByte;
		fin.read(buffer, iCopyBatch); //从文件中读取
									  //fin.read(buffer, 1024); //从文本文件中读取

		glodon_Write((const unsigned char*)buffer, iCopyBatch);//写入流中
															   //m_nPos += iCopyBatch;
															   //m_nSize += iCopyBatch;
															   //static int i = 0;
															   //std::cout << ++i << std::endl;
		iCopyByte -= iCopyBatch;
	}
	delete[] buffer;

	fin.close();

	return 0;
}


int glodon_MemoryStream::glodon_Write(const unsigned char* pBuf, int nSize, bool bSwapByte)
{
	int nWriteBytes = nSize;

	if (m_nPos + nWriteBytes >= m_nCapacity)
	{
		unsigned char* pOldBuff = m_pBuffer;
		int nIncreaseSize =
			(1 + (m_nPos + nWriteBytes - m_nCapacity) / INCREASE_MEMORY_SIZE)
			* INCREASE_MEMORY_SIZE;

		try
		{
			// 只有这里可能异常。
			m_pBuffer = new unsigned char[m_nCapacity + nIncreaseSize];
			memset(m_pBuffer, 0, m_nCapacity + nIncreaseSize);
			if (m_nPos > 0)
			{
				memcpy(m_pBuffer, pOldBuff, m_nPos);
			}
			m_nCapacity += nIncreaseSize;
			delete[] pOldBuff;
		}
		catch (...)
		{
			// new失败的 std::bad_alloc
			m_pBuffer = pOldBuff;
			// 返回缓冲区不足的部分
			return  m_nCapacity - (m_nPos + nWriteBytes);
		}
	}

	memcpy(m_pBuffer + m_nPos, pBuf, nWriteBytes);

	if (bSwapByte)
	{
		glodon_SwapBuffer(m_pBuffer + m_nPos, nWriteBytes);
	}

	m_nPos += nWriteBytes;

	if (m_nPos > m_nSize)//如果重复写，不必增加size
	{
		m_nSize += nWriteBytes;
	}

	return nWriteBytes;


}

bool glodon_MemoryStream::glodon_Reserve(int size)
{
	if (m_nCapacity >= size)
	{
		return true;
	}

	unsigned char* pOldBuff = m_pBuffer;
	int nIncreaseSize =
		(1 + (size - m_nCapacity) / INCREASE_MEMORY_SIZE)
		* INCREASE_MEMORY_SIZE;

	try
	{
		// 只有这里可能异常。
		m_pBuffer = new unsigned char[m_nCapacity + nIncreaseSize];
		memset(m_pBuffer, 0, m_nCapacity + nIncreaseSize);
		if (m_nPos > 0)
		{
			memcpy(m_pBuffer, pOldBuff, m_nPos);
		}
		m_nCapacity += nIncreaseSize;
		delete[] pOldBuff;
	}
	catch (...)
	{
		// new失败的 std::bad_alloc
		m_pBuffer = pOldBuff;
		// 返回缓冲区不足的部分
		return  false;
	}
	return true;


}


