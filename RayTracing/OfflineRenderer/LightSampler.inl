template<typename DirectionalLightType, typename IntersectionType>
Spectrum LightSampler::GetDirectionalLightLighting(DirectionalLightType *L, IntersectionType *Intersection)
{
	BWVector3D LightDir = GetDirection(Intersection->IntersectionPoint, L->GetPosition());
	BWVector3D ViewDir = Intersection->InputRay._vector;
	BWVector3D Normal = Intersection->IntersectionNormal;
	double LDotN = Dot(LightDir, Normal);
	Spectrum Color;
	if (LDotN < 0) LDotN = 0;
	Color.SetValue(0, LDotN);
	return Color;
}