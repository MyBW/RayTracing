#pragma once

template<typename DirectionLightType>
class OfflineDirectionLight : public OfflineLight
{
public:
	OfflineDirectionLight():LightSource(nullptr){ }
	DirectionLightType* GetLightSource() { return LightSource; }
	DirectionLightType* SetLightSource(DirectionLightType *InLightSource) { LightSource = InLightSource; }

	template<typename InterseciontType>
	Spectrum Sample_L(const IntersectionType &Intersection) override
	{
		if (!LightSource) return Spectrum(0.0);
		BWVector3D LightDir = GetDirection(Intersection->IntersectionPoint, LightSource->GetPosition());
		BWVector3D ViewDir = Intersection->InputRay._vector;
		BWVector3D Normal = Intersection->IntersectionNormal;
		double LDotN = Dot(LightDir, Normal);
		Spectrum Color;
		if (LDotN < 0) LDotN = 0;
		Color.SetValue(0, LDotN);
		return Color;
	}
private:
	DirectionLightType *LightSource;
};