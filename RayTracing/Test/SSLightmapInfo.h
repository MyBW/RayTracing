#pragma once
#include "json/json.h"
#include <functional>
#include <fstream>
class   glodon_MemoryStream
{
public:
	// ÿ���ڴ��С
	static const int INCREASE_MEMORY_SIZE = 4 * 1024;

public:

	glodon_MemoryStream(int nSize = 0);


	virtual ~glodon_MemoryStream()
	{
		if (m_pBuffer)
		{
			delete[] m_pBuffer;
		}
	}


	glodon_MemoryStream * glodon_Clone() const
	{
		glodon_MemoryStream *pNew = new glodon_MemoryStream(m_nSize);
		pNew->glodon_Write(m_pBuffer, m_nSize);
		return pNew;
	}

	//����һ��MemoryStream֮ǰҪReset��size��pos����0��capacity����
	void glodon_Reset()
	{
		m_nSize = 0;
		m_nPos = 0;
	}

public:

	//************************************
	// Method:    Read
	// FullName:  MemoryStream::Read
	// Access:    public 
	// Returns:   int32_t���Ѿ���ȡ�Ļ������Ĵ�С
	// Qualifier:
	// Parameter: unsigned char * pBuf��Ҫд��Ļ��������׵�ַ
	// Parameter: int32_t nSize��Ҫд��Ļ������Ĵ�С
	// Parameter: fdebool bSwapByte���Ƿ���Ҫ�ڴ���λ�������������������Ϊ���Σ�������ֽ��򽻻����������ء�
	//************************************
	inline int glodon_Read(unsigned char* pBuf, int nSize, bool bSwapByte = false)
	{
		int nReadBytes = nSize;
		if (m_nPos + nReadBytes >= m_nSize)//read���ܳ���m_nSize
		{
			nReadBytes = m_nSize - m_nPos;
		}

		memcpy(pBuf, m_pBuffer + m_nPos, nReadBytes);
		m_nPos += nReadBytes;

		if (bSwapByte)
		{
			glodon_SwapBuffer(pBuf, nSize);
		}

		return nReadBytes;
	}


	//************************************
	// Method:    Write��д���ݵ�����
	// FullName:  MemoryStream::Write
	// Access:    public 
	// Returns:   int32_t������д�����ݵĳ���
	// Qualifier:
	// Parameter: unsigned char* pBuf����Ҫ���ƻ��������׵�ַ
	// Parameter: int32_t nSize����Ҫ���Ƶ����еĳ���
	// Parameter: bool bSwapByte
	// Remark:    ����ʹ���ⲿ�ڴ��ʹ�ô�����ʽ������ڴ治�㣬����ֵ<0������ֵ��־����Ļ��岿�֡�
	//              ����ʹ��Stream�Լ�ά���ڴ�ķ�ʽ���������ڴ棬�ڴ治��Ļ�������ֵ<0������ֵ��־����Ļ��岿�֡�
	//              �ڴ治���ʱ�򣬲����ƻ��������ݡ�
	//************************************
	int glodon_Write(const unsigned char* pBuf, int nSize, bool bSwapByte = false);

	inline int glodon_Write(unsigned char* pBuf, int nSize, bool bSwapByte = false)
	{
		const unsigned char * pDest = const_cast<const unsigned char*>(pBuf);
		return glodon_Write(pDest, nSize, bSwapByte);
	}

	int glodon_writeFile(const std::string& fileName);

	int glodon_readFile(const std::string& fileName);

	//************************************
	// Method:    ReadString
	// FullName:  MemoryStream::ReadString
	// Access:    public 
	// Returns:   int32_t�����ض�ȡ�˶೤����һ�������ַ�����lenght��
	// Qualifier:
	// Parameter: fdestring & sVal����ȡ���ַ���
	// �洢��ʽΪUTF-8��д����� '\0'
	//************************************
	inline int glodon_ReadString(std::string& sVal, bool bSwapByte = false)
	{
		int nLen = 0;
		this->glodon_Read((unsigned char *)&nLen, sizeof(int), bSwapByte);
		sVal.resize(nLen);
		//unsigned short test = 0;
		//this->Read((unsigned char *)&test, 2, bSwapByte);

		this->glodon_Read((unsigned char *)sVal.c_str(), nLen);
		return sizeof(int) + nLen;
	}

	//************************************
	// Method:    WriteString
	// FullName:  MemoryStream::WriteString
	// Access:    public 
	// Returns:   int32_t
	// Qualifier:
	// Parameter: const fdestring & sVal
	// �洢��ʽΪUTF-8��д����� '\0'
	//************************************
	inline int glodon_WriteString(const std::string& sVal, bool bSwapByte = false)
	{
		int nLen = (int)sVal.size();
		this->glodon_Write((unsigned char *)&nLen, sizeof(int), bSwapByte);
		this->glodon_Write((unsigned char*)sVal.c_str(), nLen);
		return sizeof(int) + nLen;
	}

	//************************************
	// Method:    IsEnd
	// FullName:  MemoryStream::IsEnd
	// Access:    public 
	// Returns:   fdebool���Ƿ�������β��
	// Qualifier:
	//************************************
	inline bool glodon_IsEnd()
	{
		return m_nPos >= m_nSize;
	}

	//************************************
	// Method:    SwapBuffer
	// FullName:  MemoryStream::SwapBuffer
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPBYTE pBuf
	// Parameter: int32_t nSize
	// Remark:    �����ֽ���
	//************************************
	inline void glodon_SwapBuffer(unsigned char* pBuf, int nSize)
	{
		for (int i = 0; i < nSize / 2; i++)
		{
			unsigned char tmp = pBuf[i];
			pBuf[i] = pBuf[nSize - i - 1];
			pBuf[nSize - i - 1] = tmp;
		}
	}

	//************************************
	// Method:    GetBuffer
	// FullName:  MemoryStream::GetBuffer
	// Access:    public 
	// Returns:   unsigned char *������Write֮�����ʹ�õ�����ά���ڴ��ģʽ�����ֵ���ܻ�仯����Ҫʹ��ʱ��ʱ��ȡ��
	// Qualifier: const
	//************************************
	inline unsigned char * glodon_GetBuffer() const { return m_pBuffer; }
	inline int glodon_GetBufSize() const { return m_nSize; }
	inline int glodon_GetCapacity() const { return m_nCapacity; }
	inline int glodon_GetPos() const { return m_nPos; }
	inline void glodon_SetPos(int nPos) { m_nPos = nPos; }



	template <typename T>
	inline int glodon_Read(const T& buf, bool bSwapByte = false)
	{
		return glodon_Read((unsigned char*)&buf, sizeof(T), bSwapByte);
	}

	template <typename T>
	inline int glodon_Write(const T& buf, bool bSwapByte = false)
	{
		return glodon_Write((unsigned char*)&buf, sizeof(T), bSwapByte);
	}

	int glodon_ReadByte(unsigned char* pByte)
	{
		if (m_nPos + 1 >= m_nSize)//read���ܳ���m_nSize
			return 0;
		memcpy(pByte, m_pBuffer + m_nPos, 1);
		++m_nPos;

		return 1;
	}
	int glodon_WriteByte(const unsigned char& buf)
	{
		return glodon_Write((unsigned char*)&buf, 1);
	}


	bool glodon_Reserve(int size);


	unsigned char* glodon_GetBuffFromPos()
	{
		return m_pBuffer + m_nPos;
	}



protected:
	unsigned char*	m_pBuffer;
	int	m_nSize;//�û������Ĵ�С
	int	m_nCapacity;//ʵ�ʷ���Ĵ�С
	int	m_nPos;

};
typedef unsigned short int uint16;
struct SSLightmapUVInfo
{
	std::string meshName;
	int lightmapIndex;
	int uvByteOffset;
	float uvScale[2];
	float uvTranslation[2];
	int uvBufSize;
	uint16 *uvBuf;
};
class SSSceneLightmap
{
public:
	SSSceneLightmap()
	{

	}
	~SSSceneLightmap()
	{
		for (auto ssLightmapInfo : ssLightmapUVInfos)
		{
			delete ssLightmapInfo.second;
			ssLightmapInfo.second = nullptr;
		}
	}
	bool initSSSceneLightmap(const char *lightmapjsonFileName, const char *lightmapBufFileName)
	{
		if (loadSSLightMapUV(lightmapjsonFileName) && memeoryStream.glodon_readFile(std::string(lightmapBufFileName)) == 0)
		{
			parseSSLightMapUV();
			return true;
		}
		return false;
	}
	bool isHaveMeshSSLightmapUVInfo(const std::string &meshName)
	{
		std::map<std::string, SSLightmapUVInfo*>::iterator reslut = ssLightmapUVInfos.find(meshName);
		if (reslut == ssLightmapUVInfos.end() || reslut->second == nullptr)
		{
			return false;
		}
		return true;
	}
	SSLightmapUVInfo * getSSLightmapUVInfo(const std::string &meshName)
	{
		if (isHaveMeshSSLightmapUVInfo(meshName) == false)
		{
			return nullptr;
		}
		return ssLightmapUVInfos.find(meshName)->second;
	}

	bool getMeshLightmapUV(std::string &meshName, int uvSize, std::function<void(float, float)> itorFunc)
	{
		SSLightmapUVInfo *uvInfo = getSSLightmapUVInfo(meshName);
		if (uvInfo == nullptr) return false;
		return getMeshLightmapUV(uvInfo, uvSize, itorFunc);
	}
	bool getMeshLightmapUV(SSLightmapUVInfo *uvInfo, int uvSize, std::function<void(float, float)> itorFun)
	{
		const unsigned char *uvBuf = memeoryStream.glodon_GetBuffer();
		int BufSize = memeoryStream.glodon_GetBufSize();
		int curBufPosByByte = 0;
		if (uvSize * 2 * 2 + uvInfo->uvByteOffset > BufSize) // ��lightmapuv.buf �д�ŵ�����16bit���������֣����� u v �ֱ�ռ�������ֽڣ�uvSize * 2 * 2��
		{
			return false;
		}
		for (int i = 0; i < uvSize; i++)
		{
			uint16 uU = *((uint16*)(uvBuf + uvInfo->uvByteOffset + curBufPosByByte)); curBufPosByByte += 2;
			uint16 uV = *((uint16*)(uvBuf + uvInfo->uvByteOffset + curBufPosByByte)); curBufPosByByte += 2;
			itorFun(uU / 65535.0f * uvInfo->uvScale[0] + uvInfo->uvTranslation[0], uV / 65535.0f * uvInfo->uvScale[1] + uvInfo->uvTranslation[1]);
			////T pointStruct;
			//pointStruct.x = uU / 65535.0 * uvInfo->uvScale[0] + uvInfo->uvTranslation[0];
			//pointStruct.y = uV / 65535.0 * uvInfo->uvScale[1] + uvInfo->uvTranslation[1];
			//vertexsUVCoord.push_back(pointStruct);
		}
		return true;
	}


private:
	bool loadSSLightMapUV(const char *jsonFileName)
	{
		Json::Reader reader;
		std::ifstream ifs(jsonFileName);
		if (!ifs.is_open())
			return false;
		if (!reader.parse(ifs, lightmapJsonRoot, false))
		{
			return false;
		}
		return true;
	}
	void parseSSLightMapUV()
	{
		Json::Value::Members uvLayoutName = lightmapJsonRoot.getMemberNames();
		for (auto layoutName = uvLayoutName.begin(); layoutName != uvLayoutName.end(); layoutName++)
		{
			SSLightmapUVInfo* tmpLightmapInfo = new SSLightmapUVInfo;
			tmpLightmapInfo->meshName = *layoutName;
			tmpLightmapInfo->lightmapIndex = lightmapJsonRoot[*layoutName]["lightmapIdx"].asInt();
			tmpLightmapInfo->uvByteOffset = lightmapJsonRoot[*layoutName]["uv1ByteOffset"].asInt();
			for (int i = 0; i < lightmapJsonRoot[*layoutName]["uv1Scale"].size(); i++)
			{
				tmpLightmapInfo->uvScale[i] = lightmapJsonRoot[*layoutName]["uv1Scale"][i].asFloat();
			}
			for (int i = 0; i < lightmapJsonRoot[*layoutName]["uv1Translation"].size(); i++)
			{
				tmpLightmapInfo->uvTranslation[i] = lightmapJsonRoot[*layoutName]["uv1Translation"][i].asFloat();
			}
			ssLightmapUVInfos[*layoutName] = tmpLightmapInfo;
		}
	}
	glodon_MemoryStream memeoryStream;
	Json::Value lightmapJsonRoot;
	std::map<std::string, SSLightmapUVInfo*> ssLightmapUVInfos;
};
