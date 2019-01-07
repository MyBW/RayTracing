#pragma once
#include <vector>
#include "BWPrimitive.h"
#include "RTRenderer.h"
#include "RNG.h"
float AbsCosTheta(const BWVector3D &W);
bool SameHemisphere(const BWVector3D &Wi, const BWVector3D &Wo);
/*
	��������FrDiel  ��Ե��Fresnelϵ��  
	����������Cosi �������Cosֵ
			 Cost �������Cosֵ
			 Etai �������������
			 Etat �������������
*/
Spectrum FrDiel(float Cosi, float Cost, const Spectrum &Etai, const Spectrum &Etat);
/*
	��������FrCond ������Fresnelϵ��
	����������Cosi �������Cosֵ
				N ������
				K ��Ϊ������������ߣ����ǿ��Խ���������һ���� ת��Ϊ���� kΪ��������
*/
Spectrum FrCond(float Cosi, const Spectrum &Eta, const Spectrum &K);
//Fresnel ��ʾ�����ٹ���������������ı��� 
//���Է�Ϊ��Ե��͵���:  ��Ե�彫һ���֣�Fresnel������ܷ��䣬����һ���ֵĹ⣨1-Fresnel�����䵽�����ڲ�
//					   ���彫һ���֣�Fresnel������ܷ��䣬����һ����������ת��Ϊ���ܣ�����ʹ���˲���K
class Fresnel
{
public:

	virtual Spectrum Evaluate(float CosTheta) = 0;
};
class FresnelNoOp : public Fresnel
{
public:
	virtual Spectrum Evaluate(float CosTheta) override;
};
class ConductorFresnel : public Fresnel
{
public: 
	virtual Spectrum Evaluate(float CosTheta) override;
	ConductorFresnel(const Spectrum &Eta , const Spectrum &K):E(Eta), k(K) { }
private:
	Spectrum E;
	Spectrum k;
};

class DielectricFresnel : public Fresnel
{
public:
	virtual Spectrum Evaluate(float CosTheta) override;
	DielectricFresnel(const float &Ei, const float &Et):Etai(Ei),Etat(Et){ }
private:
	float Etai;
	float Etat;
};

class MicrofacetDistribution
{
public:
	 virtual ~MicrofacetDistribution() { }
	virtual float D(const BWVector3D &Wh) = 0;
	virtual void Sample_f(const BWVector3D &Wo, const BWVector3D &Wi, float u1, float u2, float *pdf) const = 0;
	virtual float Pdf(const BWVector3D &Wo, const BWVector3D &Wi) const = 0;
};
class BlinnDistribution : public MicrofacetDistribution
{
public:
	BlinnDistribution(float E):Exponent(E){ }
	float D(const BWVector3D &Wh) override;
	void Sample_f(const BWVector3D &Wo, const BWVector3D &Wi, float u1, float u2, float *pdf) const override;
	float Pdf(const BWVector3D &Wo, const BWVector3D &Wi) const override;
private:
	float Exponent;
};
class AnisotropicDistribution : public MicrofacetDistribution
{
public:
	AnisotropicDistribution(float X, float Y)
	{
		Ex = X; 
		Ey = Y;
		if (Ex > 10000.f || isnan(Ex)) Ex = 10000.f;
		if (Ey > 10000.f || isnan(Ey)) Ey = 10000.f;
	}
	float D(const BWVector3D &Wh) override;
	void Sample_f(const BWVector3D &Wo, const BWVector3D &Wi, float u1, float u2, float *pdf) const override;
	float Pdf(const BWVector3D &Wo, const BWVector3D &Wi) const override;
private: 
	float Ex;
	float Ey;
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
public:
	~Microfacet()
	{
		delete NormalD;
		delete Fr;
	}
	Microfacet(const Spectrum &Reflection, Fresnel *InFr, MicrofacetDistribution *InNormalD) :
		R(Reflection), Fr(InFr), NormalD(InNormalD)
	{

	}
	Spectrum F(const BWVector3D &Wi, const BWVector3D &Wo) const override;
	float G(const BWVector3D &Wo, const BWVector3D &Wi, const BWVector3D &Wh) const;
private:
	Spectrum R;
	Fresnel *Fr;
	MicrofacetDistribution *NormalD;
};