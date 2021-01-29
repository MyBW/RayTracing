#pragma once
#include "..\V2\Util\Spectrum.hpp"
#include "..\V2\Util\Check.h"
namespace BlackWalnut
{
	enum class TransportMode { Radiance, Importance };
	// BxDFFlags Definition
	enum class BxDFFlags {
		Unset = 0,
		Reflection = 1 << 0,
		Transmission = 1 << 1,
		Diffuse = 1 << 2,
		Glossy = 1 << 3,
		Specular = 1 << 4,
		// Composite _BxDFFlags_ definitions
		DiffuseReflection = Diffuse | Reflection,
		DiffuseTransmission = Diffuse | Transmission,
		GlossyReflection = Glossy | Reflection,
		GlossyTransmission = Glossy | Transmission,
		SpecularReflection = Specular | Reflection,
		SpecularTransmission = Specular | Transmission,
		All = Diffuse | Glossy | Specular | Reflection | Transmission

	};
	// BxDFReflTransFlags Definition
	enum class BxDFReflTransFlags {
		Unset = 0,
		Reflection = 1 << 0,
		Transmission = 1 << 1,
		All = Reflection | Transmission
	};
	inline bool IsSpecular(BxDFFlags flags) {
		return ((int)flags & (int)BxDFFlags::Specular) != 0;
	}
	struct BSDFSample
	{
		BSDFSample() = default;
		BSDFSample(const SampledSpectrum &f, const Vector3f &Wi, float Pdf, BxDFFlags Flags)
			:f(f), Wi(Wi), Pdf(Pdf), Flags(Flags)
		{

		}
		bool IsSpecular() const { return BlackWalnut::IsSpecular(Flags); }
		SampledSpectrum f;
		Vector3f Wi;
		float Pdf = 0;
		BxDFFlags Flags;
	};
	class BxDFBase
	{
	public:
		virtual BxDFFlags Flags() const = 0;
		virtual SampledSpectrum f(Vector3f Wo, Vector3f Wi, TransportMode Mode) const = 0;
		virtual BSDFSample Sample_f(Vector3f Wo, float UC, const Vector2f &W, TransportMode Mode,
			BxDFReflTransFlags SampleFlags = BxDFReflTransFlags::All) const = 0;
		virtual bool SampledPDFIsProportional()  const = 0;
		virtual float PDF(Vector3f Wo, Vector3f Wi, TransportMode Mode, BxDFReflTransFlags SampleFlags = BxDFReflTransFlags::All) const = 0;
		SampledSpectrum RHO(Vector3f Wo, std::vector<float> UC, std::vector<Vector2f> U2) const;
		SampledSpectrum RHO(std::vector<float> UC1, std::vector<Vector2f> U1, std::vector<float> UC2, std::vector<Vector2f> U2) const;
		virtual void Regularize() = 0;
	};

	

}