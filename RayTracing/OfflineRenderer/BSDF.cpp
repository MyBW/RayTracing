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

Spectrum FrDiel(float Cosi, float Cost, const Spectrum &Etai, const Spectrum &Etat)
{
	Spectrum Rparl = ((Etat * Cosi) - (Etai * Cost)) /
		((Etat * Cosi) + (Etai * Cost));
	Spectrum Rperp = ((Etai * Cosi) - (Etat * Cost)) /
		((Etai * Cosi) + (Etat * Cost));
	return (Rparl*Rparl + Rperp*Rperp) / 2.f;

}

Spectrum FrCond(float Cosi, const Spectrum &Eta, const Spectrum &K)
{
	Spectrum tmp = (Eta*Eta + K*K) * Cosi*Cosi;
	Spectrum Rparl2 = (tmp - (2.f * Eta * Cosi) + 1) /
		(tmp + (2.f * Eta * Cosi) + 1);
	Spectrum tmp_f = Eta*Eta + K*K;
	Spectrum Rperp2 =
		(tmp_f - (2.f * Eta * Cosi) + Cosi*Cosi) /
		(tmp_f + (2.f * Eta * Cosi) + Cosi*Cosi);
	return (Rparl2 + Rperp2) / 2.f;
}

void BSDF::SetLocalSystem(const BWVector3D & X, const BWVector3D & Y, const BWVector3D & Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
	WorldToLocalMatrix = BWMatrix4(X, Y, Z);
	WorldToLocalMatrix = WorldToLocalMatrix.transpose();
}

BWVector3D BSDF::WorldToLocal(const BWVector3D &V) const
{
	return WorldToLocalMatrix * V;
}

BWVector3D BSDF::LocalToWorld(const BWVector3D &V) const
{
	return WorldToLocalMatrix.transpose() * V;
}

void BSDF::AddBXDF(BXDF *NewBxDF)
{
	if (NewBxDF)
	{
		BXDFs.push_back(NewBxDF);
	}
}


Spectrum BSDF::Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float &pdf, const BSDFSample& BSDFSampleData, BXDF_TYPE &SampleType, BXDF_TYPE Flags /*= BXDF_TYPE::BXDF_ALL*/) const
{
	return Spectrum();
}

float BSDF::Pdf(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	return 0;
}

Spectrum BSDF::F(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	Spectrum Color;
	BWVector3D LocalWo = WorldToLocal(Wo);
	BWVector3D LocalWi = WorldToLocal(Wi);
	for (int i = 0; i < BXDFs.size(); i++)
	{
		Color += BXDFs[i]->F(LocalWi,LocalWo);
	}
	return Color;
}

Spectrum BSDF::RHO(const BWVector3D &Wo, RNG &Rng, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	
}

Spectrum BSDF::RHO(RNG &Rng, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{

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

Spectrum Microfacet::F(const BWVector3D &Wi, const BWVector3D &Wo) const
{
	BWVector3D Wh;
	Wh = (Wi + Wo);
	Wh.normalize();
	float CosThetaO = AbsCosTheta(Wo);
	float CosThetaI = AbsCosTheta(Wi);
	if (CosThetaO == 0.0f || CosThetaI == 0.0f) return Spectrum(0.0);
	float CosTheatH = Dot(Wi, Wh);
	Spectrum Fvalue = Fr->Evaluate(CosTheatH);
	return R * NormalD->D(Wh) * Fvalue * G(Wo, Wi, Wh) / (4 * CosThetaO * CosThetaI);
}

float Microfacet::G(const BWVector3D &Wo, const BWVector3D &Wi, const BWVector3D &Wh) const
{
	float NdotWh = AbsCosTheta(Wh);
	float NdotWo = AbsCosTheta(Wo);
	float NdotWi = AbsCosTheta(Wi);
	float WodotWh = fabsf(Dot(Wo, Wh));
	return TMin(1.0f, TMin((2.0f*NdotWh*NdotWo) / WodotWh, (2.0f*NdotWh*NdotWi) / WodotWh));
}


Spectrum ConductorFresnel::Evaluate(float CosTheta)
{
	return FrCond( fabsf(CosTheta), E, k);
}

Spectrum DielectricFresnel::Evaluate(float CosTheta)
{
	CosTheta = Clamp(CosTheta, -1.0f, 1.0f);
	bool IsInside = CosTheta > 0.0f;
	float Ei = Etai, Et = Etat;
	if (!IsInside) Swap(Ei, Et);
	// Compute _sint_ using Snell's law
	float sint = Ei / Et * sqrtf(TMax(0.f, 1.f - CosTheta*CosTheta));
	if (sint >= 1.0f)
		return Spectrum(1.0f);
	else
	{
		float Cost = sqrtf(TMax(0.f, 1.f - sint*sint));
		return FrDiel(CosTheta, Cost, Ei, Et);
	}
}
Spectrum FresnelNoOp::Evaluate(float CosTheta)
{
	return Spectrum(1.0f);
}

float BlinnDistribution::D(const BWVector3D &Wh)
{
	float costhetah = AbsCosTheta(Wh);
	return (Exponent + 2) * INV_TWOPI * powf(costhetah, Exponent);
}

void BlinnDistribution::Sample_f(const BWVector3D &Wo, const BWVector3D &Wi, float u1, float u2, float *pdf) const
{

}

float BlinnDistribution::Pdf(const BWVector3D &Wo, const BWVector3D &Wi) const
{
	return 0;
}

float AnisotropicDistribution::D(const BWVector3D &Wh)
{
	float costhetah = AbsCosTheta(Wh);
	float d = 1.f - costhetah * costhetah;
	if (d == 0.f) return 0.f;
	float e = (Ex * Wh.x * Wh.x + Ey * Wh.y * Wh.y) / d;
	return sqrtf((Ex + 2.f) * (Ey + 2.f)) * INV_TWOPI * powf(costhetah, e);
}

void AnisotropicDistribution::Sample_f(const BWVector3D &Wo, const BWVector3D &Wi, float u1, float u2, float *pdf) const
{

}

float AnisotropicDistribution::Pdf(const BWVector3D &Wo, const BWVector3D &Wi) const
{
	return 0.0;
}


