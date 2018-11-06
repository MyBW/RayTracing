#include "BSDF.h"

Spectrum BXDF::Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float u1, float u2, float &pdf) const
{
	Wi = ConsineSampleHemisphere(u1, u2);
	if (Wi.z < 0.0f) Wi.z *= -1.0f;
	pdf = Pdf(Wo, Wi);
	return F(Wo, Wi);
}

float BXDF::Pdf(const BWVector3D &Wo, BWVector3D &Wi) const
{
	SameHemisphere(Wi, Wo) ? AbsCosTheta(Wi) / 3.1415 : 0.0f;
}

Spectrum Lambertian::F(const BWVector3D &Wi, const BWVector3D &Wo) const
{
	return R * INV_PI;
}
