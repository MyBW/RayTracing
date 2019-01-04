#include "RTMaterial.h"

void RTMaterial::CreateBSDF(const IntersectionInfo& Intersection, BSDF &Bsdf) const
{
	Lambertian* Lamber = new Lambertian();
	Spectrum Color;
	Color.SetValue(0, 1.0);
	Lamber->SetColor(Color);
	Bsdf.AddBXDF(Lamber);
}
