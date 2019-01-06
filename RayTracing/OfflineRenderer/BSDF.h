#pragma once
#include <vector>
#include "BWPrimitive.h"
#include "RTRenderer.h"
#include "RNG.h"
float AbsCosTheta(const BWVector3D &W);
bool SameHemisphere(const BWVector3D &Wi, const BWVector3D &Wo);

class Fresnel
{
public:

	Spectrum Evaluate(float CosTheta);
};
class MicrofacetDistribution
{
public:
	virtual float D(const BWVector3D &Wh);
};
class BlinnDistribution : public MicrofacetDistribution
{
public:
	float D(const BWVector3D &Wh) override;
};
class AnisotropicDistribution : public MicrofacetDistribution
{
public:
	float D(const BWVector3D &Wh) override;
};


class BXDF;
enum BXDF_TYPE
{
	BXDF_ALL
};
class BSDFSample
{
	float Dir[2];
	float Component;
};
class BSDF
{
public:
	~BSDF()
	{
		for (int i = 0; i < BXDFs.size(); i++)
		{
			delete BXDFs[i];
		}
	}
	void AddBXDF(BXDF *NewBxDF);
	Spectrum Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float &pdf, const BSDFSample& BSDFSampleData, BXDF_TYPE &SampleType , BXDF_TYPE Flags = BXDF_TYPE::BXDF_ALL) const;
	float Pdf(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	Spectrum F(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	Spectrum RHO(const BWVector3D &Wo, RNG &Rng, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	Spectrum RHO(RNG &Rng, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	std::vector<BXDF*> BXDFs;
	
protected:
};

class BXDF
{
public:
   virtual Spectrum F(const BWVector3D &Wi, const BWVector3D &Wo) const = 0;
   virtual Spectrum Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float u1, float u2, float &pdf) const;
   virtual float Pdf(const BWVector3D &Wo, BWVector3D &Wi) const;
protected:
};

class Lambertian : public BXDF
{
public:
	Spectrum F(const BWVector3D &Wi, const BWVector3D &Wo) const override;
	inline void SetColor(const Spectrum &R) { this->R = R; }
private:
	Spectrum R;
};

class Microfacet : public BXDF
{

};
// 一种微表面漫反射模型
class OrenNayarMicrofacet : public Microfacet
{
public:
private:
};
//一种微表面高光反射模型
class TorranceSparrowMicorfacet : public Microfacet
{
public:
	TorranceSparrowMicorfacet(const Spectrum &Reflection, Fresnel *InFr, MicrofacetDistribution *InD) :
		R(Reflection), Fr(InFr), D(InD)
	{

	}
	Spectrum F(const BWVector3D &Wi, const BWVector3D &Wo) const override;
	float G(const BWVector3D &Wo, const BWVector3D &Wi, const BWVector3D &Wh);
private:
	Spectrum R;
	Fresnel *Fr;
	MicrofacetDistribution *D;
};