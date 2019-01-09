#include "RTMaterial.h"
#include "Renderer.h"

void RTMaterial::CreateBSDF(const IntersectionInfo& Intersection, BSDF &Bsdf) const
{
	/*Lambertian* Lamber = new Lambertian();
	Spectrum Color;
	Color.SetValue(0, 1.0);
	Lamber->SetColor(Color);
	Bsdf.AddBXDF(Lamber);
	Bsdf.SetLocalSystem(Intersection.IntersectionBiNormal, Intersection.IntersectionTangent, Intersection.IntersectionNormal);*/


	Bsdf.SetLocalSystem(Intersection.IntersectionTangent, Intersection.IntersectionBiNormal, Intersection.IntersectionNormal);

	Spectrum Color(1.0);
	BlinnDistribution *Blinn = new BlinnDistribution(20);
	DielectricFresnel *Fre = new DielectricFresnel(1.0, 1.31); // ´ÓÕæ¿Õµ½±ù
	//ConductorFresnel *Fre = new ConductorFresnel(Spectrum(1.0), Spectrum(1.0));
	//FresnelNoOp *Fre = new FresnelNoOp();
	Microfacet* Micro = new Microfacet(Color, Fre, Blinn);
	Bsdf.AddBXDF(Micro);
	

	Lambertian* Lamber = new Lambertian();
	Color.SetValue(0, 1.0);
	Color.SetValue(1, 1.0);
	Color.SetValue(2, 1.0);
	Lamber->SetColor(Color);
	//Bsdf.AddBXDF(Lamber);
}
