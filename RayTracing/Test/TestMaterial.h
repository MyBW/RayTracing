#pragma once
#include "..\OfflineRenderer\RTMaterial.h"
#include "..\OfflineRenderer\Renderer.h"
#include "..\OfflineRenderer\BSDF.h"



class LambertianAndMicrofaceMateial : public RTMaterial
{
public:
	LambertianAndMicrofaceMateial(Spectrum &InBaseColor) :BaseColor(InBaseColor){ }
	void CreateBSDF(const IntersectionInfo& Intersection, BSDF &Bsdf) const override
	{
		Bsdf.SetLocalSystem(Intersection.IntersectionTangent, Intersection.IntersectionBiNormal, Intersection.IntersectionNormal);

		Spectrum Color(1.0);
		//BlinnDistribution *Blinn = new BlinnDistribution(20);
		//DielectricFresnel *Fre = new DielectricFresnel(1.0, 1.31); // ´ÓÕæ¿Õµ½±ù
		//														   //ConductorFresnel *Fre = new ConductorFresnel(Spectrum(1.0), Spectrum(1.0));
		//														   //FresnelNoOp *Fre = new FresnelNoOp();
		//Microfacet* Micro = new Microfacet(Color, Fre, Blinn);
		//Bsdf.AddBXDF(Micro);


		Lambertian* Lamber = new Lambertian();
		Color.SetValue(0, 0.1);
		Color.SetValue(1, 0.1);
		Color.SetValue(2, 0.1);
		Lamber->SetColor(BaseColor);
		Bsdf.AddBXDF(Lamber);
	}
private:
	Spectrum BaseColor;
};
class EmitMaterial : public RTMaterial
{
public:
	EmitMaterial(const Spectrum &InEmitColor):EmitColor(InEmitColor){ }
	void CreateBSDF(const IntersectionInfo& Intersection, BSDF &Bsdf) const override
	{
		Bsdf.SetLocalSystem(Intersection.IntersectionTangent, Intersection.IntersectionBiNormal, Intersection.IntersectionNormal);

		SelfEmit *SelfEmitBXDF = new SelfEmit();
		SelfEmitBXDF->SetEmit(EmitColor);
		Bsdf.AddBXDF(SelfEmitBXDF);
	}
private:
	Spectrum EmitColor;
};