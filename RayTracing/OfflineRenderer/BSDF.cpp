#include "BSDF.h"
#include "Montecarlo.h"
#include "RTMaterial.h"
#include "Sample.h"
#include "RNG.h"
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
	//int comp = std::min((int)std::floor(u[0] * matchingComps), matchingComps - 1);
	//Point2f uRemapped(std::min(u[0] * matchingComps - comp, OneMinusEpsilon),u[1]);
	int matchingComps = BXDFs.size();
	int comp = TMin((int)std::floor(BSDFSampleData.Dir[0] * matchingComps), matchingComps - 1);
	BWPoint2DD uRemapped(TMin(BSDFSampleData.Dir[0] * matchingComps - comp, OneMinusEpsilon), BSDFSampleData.Dir[1]);

	Spectrum F;
	BWVector3D LocalWo = WorldToLocal(Wo);
	BWVector3D LocalWi;
	BXDF *MainBxdf = nullptr;
	if (LocalWo.z == 0) return 0.0f;
	pdf = 0.0f;
	MainBxdf = BXDFs[0];
	F = BXDFs[0]->Sample_F(LocalWo, LocalWi, uRemapped.x, uRemapped.y, pdf);
	if (pdf == 0.f)
	{
		return 0.f;
	}
	Wi = LocalToWorld(LocalWi);
	for (int i = 0 ;i < BXDFs.size() ; i++)
	{
		if (BXDFs[i] != MainBxdf)
		{
			pdf += BXDFs[i]->Pdf(LocalWo, LocalWi);
			F += BXDFs[i]->F(LocalWi, LocalWo);
		}
	}
	pdf /= BXDFs.size();
	return F;
}

float BSDF::Pdf(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	float ReslutPdf = 0.0f;
	BWVector3D LocalWo = WorldToLocal(Wo);
	BWVector3D LocalWi = WorldToLocal(Wi);
	LocalWo.normalize();
	LocalWi.normalize();
	for (int i = 0; i < BXDFs.size(); i++)
	{
		ReslutPdf += BXDFs[i]->Pdf(LocalWo, LocalWi);
	}
	return ReslutPdf / BXDFs.size();
}

Spectrum BSDF::F(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	Spectrum Color;
	BWVector3D LocalWo = WorldToLocal(Wo);
	BWVector3D LocalWi = WorldToLocal(Wi);
	LocalWo.normalize();
	LocalWi.normalize();
	for (int i = 0; i < BXDFs.size(); i++)
	{
		Color += BXDFs[i]->F(LocalWi,LocalWo);
	}
	return Color;
}

Spectrum BSDF::Le(const BWVector3D &Wo) const
{
	Spectrum Color;
	BWVector3D LocalWo = WorldToLocal(Wo);
	LocalWo.normalize();
	for (int i = 0; i < BXDFs.size(); i++)
	{
		Color += BXDFs[i]->Le(LocalWo);
	}
	return Color;
}

Spectrum BSDF::RHO(const BWVector3D &Wo, RNG &Rng, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	return Spectrum();
}

Spectrum BSDF::RHO(RNG &Rng, BXDF_TYPE Flag /*= BXDF_TYPE::BXDF_ALL*/) const
{
	return Spectrum();
}

Spectrum BXDF::Le(const BWVector3D &Wo)
{
	return Lemit * AbsCosTheta(Wo);
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
	return SameHemisphere(Wi, Wo) ? AbsCosTheta(Wi) * INV_PI : 0.0f;
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


float Microfacet::Pdf(const BWVector3D &Wo, BWVector3D &Wi) const
{
	if (!SameHemisphere(Wo, Wi)) return 0.f;
	return NormalD->Pdf(Wo, Wi);
}

Spectrum Microfacet::Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float u1, float u2, float &pdf) const
{
	if (Wo.z == 0) return 0.f;
	BWVector3D wh = NormalD->Sample_wh(Wo, u1, u2);
	Wi = Reflect(Wo, wh);
	if (!SameHemisphere(Wo, Wi)) return Spectrum(0.f);

	// Compute PDF of _wi_ for microfacet reflection
	pdf = NormalD->Pdf(Wo, wh) / (4 * Dot(Wo, wh));
	return F(Wo, Wi);
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
	BWVector3D Wh = Wo + Wi;
	Wh.normalize();
	float costheta = AbsCosTheta(Wh);
	// Compute PDF for $\wi$ from Blinn distribution
	float blinn_pdf = ((Exponent + 1.f) * powf(costheta, Exponent)) /
		(2.f * PI * 4.f * Dot(Wo, Wh));
	if (Dot(Wo, Wh) <= 0.f) blinn_pdf = 0.f;
	return blinn_pdf;
}

BWVector3D BlinnDistribution::Sample_wh(const BWVector3D &wo, const float u, float v) const
{
	float costheta = powf(u, 1.f / (Exponent + 1));
	float sintheta = sqrtf(TMax(0.f, 1.f - costheta*costheta));
	float phi = u * 2.f * PI;
	BWVector3D wh = SphericalDirection(sintheta, costheta, phi);
	if (!SameHemisphere(wo, wh)) wh = -wh;
	return wh;
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
	BWVector3D Wh = Wo + Wi;
	Wh.normalize();
	float costheta = AbsCosTheta(Wh);
	float ds = 1.f - costheta * costheta;
	float anisotropic_pdf = 0.f;
	if (ds > 0.f && Dot(Wo, Wh) > 0.f) {
		float e = (Ex * Wh.x * Wh.x + Ey * Wh.y * Wh.y) / ds;
		float d = sqrtf((Ex + 1.f) * (Ey + 1.f)) * INV_TWOPI *
			powf(costheta, e);
		anisotropic_pdf = d / (4.f * Dot(Wo, Wh));
	}
	return anisotropic_pdf;
}


BWVector3D AnisotropicDistribution::Sample_wh(const BWVector3D &wo, const float u, float v) const
{
	float costheta = powf(u, 1.f / (20 + 1));
	float sintheta = sqrtf(TMax(0.f, 1.f - costheta*costheta));
	float phi = u * 2.f * PI;
	BWVector3D wh = SphericalDirection(sintheta, costheta, phi);
	if (!SameHemisphere(wo, wh)) wh = -wh;
	return wh;
}

BSDFSampleOffset::BSDFSampleOffset(Sample &InSample, int SampleNum)
{
	this->SampleNum = SampleNum;
	BSDFComponentOffset = InSample.Add1D(SampleNum);
	DirOffset = InSample.Add2D(SampleNum);
}

BSDFSample::BSDFSample(Sample &InSample, const BSDFSampleOffset &Offsets, int SampleIndex)
{
	assert(SampleIndex < InSample.N1D[Offsets.BSDFComponentOffset]);
	assert(SampleIndex < InSample.N2D[Offsets.DirOffset]);
	Component = InSample.N1Data[Offsets.BSDFComponentOffset][SampleIndex];
	Dir[0] = InSample.N2Data[Offsets.DirOffset][2 * SampleIndex];
	Dir[1] = InSample.N2Data[Offsets.DirOffset][2 * SampleIndex + 1];
	assert(Component<1.0f && Component > 0.0f);
	assert(Dir[0] < 1.0f && Dir[0] >= 0.0f);
	assert(Dir[1] < 1.0f && Dir[1] >= 0.0f);
}

BSDFSample::BSDFSample(RNG &Rng)
{
	Component = Rng.GetRandomFloat();
	Dir[0] = Rng.GetRandomFloat();
	Dir[1] = Rng.GetRandomFloat();
}

Spectrum TestBSDF::F(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag /* = BXDF_TYPE::BXDF_ALL */) const
{
	BWVector3D Wh;
	Wh = (Wi + Wo);
	Wh.normalize();
	return Spectrum(1.0) * TMax( Dot(Wh, Normal) , 0);
}
