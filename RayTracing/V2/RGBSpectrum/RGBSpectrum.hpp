#pragma once

namespace BlackWalnut
{
	extern const int sRGBToSpectrumTable_Res;
	extern const float sRGBToSpectrumTable_Scale[64];
	extern const float sRGBToSpectrumTable_Data[2359296];

	extern const int DCI_P3ToSpectrumTable_Res;
	extern const float DCI_P3ToSpectrumTable_Scale[64];
	extern const float DCI_P3ToSpectrumTable_Data[2359296];

	extern const int REC2020ToSpectrumTable_Res;
	extern const float REC2020ToSpectrumTable_Scale[64];
	extern const float REC2020ToSpectrumTable_Data[2359296];

	extern const int ACES2065_1ToSpectrumTable_Res;
	extern const float ACES2065_1ToSpectrumTable_Scale[64];
	extern const float ACES2065_1ToSpectrumTable_Data[2359296];
}