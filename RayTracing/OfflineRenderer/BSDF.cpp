#include "BSDF.h"
#include "Montecarlo.h"
#include "RTMaterial.h"
float AbsCosTheta(const BWVector3D &W)
{
	return fabsf(W.z);
}
bool SameHemisphere(const BWVector3D &Wi, const BWVector3D &Wo)
{
	return Wi.z * Wo.z > 0.0f;
}

void BSDF::AddBXDF(BXDF *NewBxDF)
{
	if (NewBxDF)
	{
		BXDFs.push_back(NewBxDF);
	}
}


Spectrum BSDF::F(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	Spectrum Color;
	for (int i = 0; i < BXDFs.size(); i++)
	{
		Color = BXDFs[i]->F(Wi, Wo);
	}
	return Color;
}

Spectrum BXDF::Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float u1, float u2, float &pdf) const
{
	Wi = ConsineSampleHemisphere(u1, u2);
	if (Wi.z < 0.0f) Wi.z *= -1.0f;
	pdf = Pdf(Wo, Wi);
	return F(Wo, Wi);
}

float BXDF::Pdf(const BWVector3D &Wo, BWVector3D &Wi) const
{
	return SameHemisphere(Wi, Wo) ? AbsCosTheta(Wi) / 3.1415 : 0.0f;
}

Spectrum Lambertian::F(const BWVector3D &Wi, const BWVector3D &Wo) const
{
	return R * INV_PI;
}
