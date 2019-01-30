#pragma once
#include <vector>
#include "BWPrimitive.h"
#include "RTRenderer.h"
class RNG;
class Sample;
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
				Eta ������
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
	virtual BWVector3D Sample_wh(const BWVector3D &wo, const float u , float v) const = 0;
};
class BlinnDistribution : public MicrofacetDistribution
{
public:
	BlinnDistribution(float E):Exponent(E){ }
	float D(const BWVector3D &Wh) override;
	void Sample_f(const BWVector3D &Wo, const BWVector3D &Wi, float u1, float u2, float *pdf) const override;
	float Pdf(const BWVector3D &Wo, const BWVector3D &Wi) const override;
	// Test Version
	BWVector3D Sample_wh(const BWVector3D &wo, const float u, float v) const override;
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
	// Test Version
	BWVector3D Sample_wh(const BWVector3D &wo, const float u, float v) const override;
private: 
	float Ex;
	float Ey;
};


class BXDF;
enum BXDF_TYPE
{
	BSDF_SPECULAR = 1 << 4,
	BXDF_ALL
};
class BSDFSampleOffset
{
public:
	BSDFSampleOffset(Sample &InSample, int SampleNum);
	int SampleNum;
	int BSDFComponentOffset;
	int DirOffset;

};
class BSDFSample
{
public:
	BSDFSample(Sample &InSample, const BSDFSampleOffset &Offsets, int SampleIndex);
	BSDFSample(RNG &Rng);
	BSDFSample() :Component(0.0f) { Dir[0] = 0.0f; Dir[1] = 0.0f; }
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
	Spectrum Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float &pdf, const BSDFSample& BSDFSampleData, BXDF_TYPE &SampleType , BXDF_TYPE Flags = BXDF_TYPE::BXDF_ALL) const ;
	float Pdf(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	virtual Spectrum F(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	virtual Spectrum Le(const BWVector3D &Wo) const;
	Spectrum RHO(const BWVector3D &Wo, RNG &Rng, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	Spectrum RHO(RNG &Rng, BXDF_TYPE Flag = BXDF_TYPE::BXDF_ALL) const;
	std::vector<BXDF*> BXDFs;
	void SetLocalSystem(const BWVector3D & X, const BWVector3D & Y, const BWVector3D & Z);
	BWVector3D WorldToLocal(const BWVector3D &V) const;
	BWVector3D LocalToWorld(const BWVector3D &V) const;
protected:
	BWVector3D X;
	BWVector3D Y;
	BWVector3D Z;
	BWMatrix4 WorldToLocalMatrix;
};

class TestBSDF : public BSDF
{
public:
	virtual Spectrum F(const BWVector3D &Wo, const BWVector3D &Wi, BXDF_TYPE Flag  = BXDF_TYPE::BXDF_ALL ) const override;
	BWVector3D Normal;
};

class BXDF
{
public:
	BXDF():Lemit(0.0f){ }
   void SetLe(Spectrum InLe) { Lemit = InLe; }
   virtual Spectrum F(const BWVector3D &Wi, const BWVector3D &Wo) const = 0;
   virtual Spectrum Le(const BWVector3D &Wo);
   virtual Spectrum Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float u1, float u2, float &pdf) const;
   virtual float Pdf(const BWVector3D &Wo, BWVector3D &Wi) const;
protected:
	Spectrum Lemit;
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
	float Pdf(const BWVector3D &Wo, BWVector3D &Wi) const ;
	Spectrum Sample_F(const BWVector3D &Wo, BWVector3D &Wi, float u1, float u2, float &pdf) const override;
private:
	Spectrum R;
	Fresnel *Fr;
	MicrofacetDistribution *NormalD;
};

class SelfEmit : public BXDF
{
public: 
	void SetEmit(Spectrum LEmit)
	{
		this->Lemit = LEmit;
	}
	Spectrum F(const BWVector3D &Wi, const BWVector3D &Wo) const
	{
		return Spectrum(0.0);
	}
};
