#include "RTMaterial.h"

void RTMaterial::CreateBSDF(const IntersectionInfo& Intersection, BSDF &Bsdf) const
{
	/*Lambertian* Lamber = new Lambertian();
	Spectrum Color;
	Color.SetValue(0, 1.0);
	Lamber->SetColor(Color);
	Bsdf.AddBXDF(Lamber);*/
	Spectrum Color(1.0);
	BlinnDistribution *Blinn = new BlinnDistribution(20);
	//DielectricFresnel *Fre = new DielectricFresnel(1.0, 1.31); // ´ÓÕæ¿Õµ½±ù
	//ConductorFresnel *Fre = new ConductorFresnel()
	FresnelNoOp *Fre = new FresnelNoOp();
	Microfacet* Micro = new Microfacet(Color, Fre, Blinn);
	Bsdf.AddBXDF(Micro);
}
