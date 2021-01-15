#include "Color.hpp"
#include "../RGBSpectrum/RGBSpectrum.hpp"
using namespace BlackWalnut;
extern const int sRGBToSpectrumTable_Res;
extern const float sRGBToSpectrumTable_Scale[64];
extern const float sRGBToSpectrumTable_Data[2359296];
const RGBToSpectrumTable *RGBToSpectrumTable::sRGB;

extern const int DCI_P3ToSpectrumTable_Res;
extern const float DCI_P3ToSpectrumTable_Scale[64];
extern const float DCI_P3ToSpectrumTable_Data[2359296];

const RGBToSpectrumTable *RGBToSpectrumTable::DCI_P3;

extern const int REC2020ToSpectrumTable_Res;
extern const float REC2020ToSpectrumTable_Scale[64];
extern const float REC2020ToSpectrumTable_Data[2359296];

const RGBToSpectrumTable *RGBToSpectrumTable::Rec2020;

extern const int ACES2065_1ToSpectrumTable_Res;
extern const float ACES2065_1ToSpectrumTable_Scale[64];
extern const float ACES2065_1ToSpectrumTable_Data[2359296];

const RGBToSpectrumTable *RGBToSpectrumTable::ACES2065_1;

void BlackWalnut::RGBToSpectrumTable::Init()
{
		
	sRGB = new RGBToSpectrumTable(sRGBToSpectrumTable_Res, sRGBToSpectrumTable_Scale, sRGBToSpectrumTable_Data);
	DCI_P3 = new RGBToSpectrumTable(DCI_P3ToSpectrumTable_Res, DCI_P3ToSpectrumTable_Scale, DCI_P3ToSpectrumTable_Data);
	Rec2020 = new RGBToSpectrumTable(REC2020ToSpectrumTable_Res, REC2020ToSpectrumTable_Scale, REC2020ToSpectrumTable_Data);
	ACES2065_1 = new RGBToSpectrumTable(ACES2065_1ToSpectrumTable_Res, ACES2065_1ToSpectrumTable_Scale, ACES2065_1ToSpectrumTable_Data);
}

