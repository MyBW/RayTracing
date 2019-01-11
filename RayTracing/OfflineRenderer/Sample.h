#pragma once
#include <vector>
class Sample
{
public:
	float ImageX;
	float ImageY;
	
	Sample():DataMem(NULL)
	{
	}
	Sample(const Sample& Src)
	{
		DataMem = nullptr;
		N1D = Src.N1D;
		N2D = Src.N2D;
		AllocDataMemory();
	}
	~Sample()
	{
		if(DataMem) delete[] DataMem;
	}
	int Add1D(int Num)
	{
		N1D.push_back(Num);
		return N1D.size() - 1;
	}
	int Add2D(int Num)
	{
		N2D.push_back(Num);
		return N2D.size() - 1;
	}
	std::vector<Sample*> Duplicate(int DumplicateNum)
	{
		std::vector<Sample*> Samples;
		for (size_t i = 0; i < DumplicateNum; i++)
		{
			Samples.push_back(new Sample(*this));
		}
		return Samples;
	}
	std::vector<int> N1D;
	std::vector<int> N2D;
	std::vector<float*>N1Data;
	std::vector<float*>N2Data;
	float *DataMem;
private:
	void ClearMemData()
	{
		if (DataMem)
		{
			delete[] DataMem;
			N1Data.clear();
			N2Data.clear();
			DataMem = NULL;
		}
	}
	void AllocDataMemory()
	{
		ClearMemData();
		int TotSampleDataSize = 0;
		N1Data.resize(N1D.size());
		N2Data.resize(N2D.size());
		for (int i = 0; i < N1D.size(); i++)
		{
			TotSampleDataSize += N1D[i];
		}
		for (int i = 0; i < N2D.size();i++)
		{
			TotSampleDataSize += N2D[i] * 2;
		}
		if (TotSampleDataSize == 0) return;

		DataMem = new float[TotSampleDataSize];
		int OffSet = 0;
		for (int i = 0; i < N1D.size();i++)
		{
			N1Data[i] = DataMem + OffSet;
			OffSet += N1D[i];
		}
		for (int i = 0; i < N2D.size();i++)
		{
			N2Data[i] = DataMem + OffSet;
			OffSet += N2D[i] * 2;
		}
	}
};