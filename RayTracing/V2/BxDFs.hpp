#pragma once
#include "Base/BxDF.hpp"
#include "Math/Geom.hpp"
#include "Util/Spectrum.hpp"
#include "Base/BSDF.hpp"
#include "Util/Scattering.hpp"
namespace BlackWalnut
{
	class IdealDiffuseBxDF : public BxDFBase
	{
	public:
		IdealDiffuseBxDF() = default;
		IdealDiffuseBxDF(SampledSpectrum InR) :R(InR) {}
		SampledSpectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const override
		{
			if (!SameHemisphere(wo, wi))
				return SampledSpectrum(0.f);
			return R * InvPi;
		}
		BSDFSample Sample_f(Vector3f Wo, float UC, const Vector2f &W, TransportMode Mode, BxDFReflTransFlags SampleFlags /* = BxDFReflTransFlags::All */) const
		{
			if (!((int)SampleFlags & (int)BxDFReflTransFlags::Reflection))
				return{};
			Vector3f wi = SampleCosineHemisphere(W);
			if (Wo.Z < 0)
				wi.Z *= -1;
			float pdf = AbsCosTheta(wi) * InvPi;
			return BSDFSample(f(Wo, wi, Mode), wi, pdf, BxDFFlags::DiffuseReflection);
		}
		float PDF(Vector3f Wo, Vector3f Wi, TransportMode Mode, BxDFReflTransFlags SampleFlags /* = BxDFReflTransFlags::All */) const 
		{
			if (!((int)SampleFlags & (int)BxDFReflTransFlags::Reflection))
				return 0;
			if (SameHemisphere(Wo, Wi))
				return AbsCosTheta(Wi) * InvPi;
			else
				return 0;
		}
		bool SampledPDFIsProportional() const { return false; }
		void Regularize() {}
		BxDFFlags Flags() const 
		{
			return R ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset;
		}
	private:
		SampledSpectrum R;
	};

	class DiffuseBxDF : public BxDFBase
	{
	public:
		DiffuseBxDF() = default;
		DiffuseBxDF(const SampledSpectrum &R, const SampledSpectrum &T, float Sigma):
			R(R),T(T)
		{
			float Sigma2 = Pow(Radians(Sigma), 2);
			A = 1 - Sigma2 / (2 * (Sigma2 + 0.33f));
			B = 0.45f * Sigma2 / (Sigma2 + 0.09f);
		}
		SampledSpectrum f(Vector3f Wo, Vector3f Wi, TransportMode Mode) const override
		{
			if(B == 0)
				return SameHemisphere(Wo, Wi) ? (R * InvPi) : (T * InvPi);

			if ((SameHemisphere(Wo, Wi) && !R) || (!SameHemisphere(Wo, Wi) && !T))
				return SampledSpectrum(0.);
			
			float SinTheta_i = SinTheta(Wi), SinTheta_o = SinTheta(Wo);

			// Compute cosine term of Oren--Nayar model
			float MaxCos = 0;
			if (SinTheta_i > 0 && SinTheta_o > 0)
				MaxCos = std::max<float>(0, CosDPhi(Wi, Wo));

			// Compute sine and tangent terms of Oren--Nayar model
			float SinAlpha, TanBeta;
			if (AbsCosTheta(Wi) > AbsCosTheta(Wo))
			{
				SinAlpha = SinTheta_o;
				TanBeta = SinTheta_i / AbsCosTheta(Wi);
			}
			else
			{
				SinAlpha = SinTheta_i;
				TanBeta = SinTheta_o / AbsCosTheta(Wo);
			}
			
			if(SameHemisphere(Wi,Wo))
				return R * InvPi * (A + B * MaxCos * SinAlpha * TanBeta);
			else
				return T * InvPi * (A + B * MaxCos * SinAlpha * TanBeta);

		}
		BSDFSample Sample_f(Vector3f Wo, float UC, const Vector2f &W, TransportMode Mode, BxDFReflTransFlags SampleFlags /* = BxDFReflTransFlags::All */) const override
		{
			float PR = R.MaxComponentValue();
			float PT = T.MaxComponentValue();
			if (!((int)SampleFlags & (int)BxDFReflTransFlags::Reflection)) PR = 0;
			if (!((int)SampleFlags & (int)BxDFReflTransFlags::Transmission)) PT = 0;
			if (PR == 0 && PT == 0) return{};
			float CPDF;
			if (SampleDiscrete({ PR, PT }, UC, &CPDF) == 0)
			{
				Vector3f Wi = SampleCosineHemisphere(W);
				if (Wo.Z < 0)
					Wi.Z = Wi.Z * -1;
				float Pdf = AbsCosTheta(Wi) * InvPi * CPDF;
				return BSDFSample(f(Wo, Wi, Mode), Wi, Pdf, BxDFFlags::DiffuseReflection);
			}
			else
			{
				Vector3f Wi = SampleCosineHemisphere(W);
				if (Wo.Z > 0)
					Wi.Z = Wi.Z * -1;
				float Pdf = AbsCosTheta(Wi) * InvPi * CPDF;
				return BSDFSample(f(Wo, Wi, Mode), Wi, Pdf, BxDFFlags::DiffuseTransmission);
			}
		}
		float PDF(Vector3f Wo, Vector3f Wi, TransportMode Mode, BxDFReflTransFlags SampleFlags /* = BxDFReflTransFlags::All */) const override
		{
			float PR = R.MaxComponentValue();
			float PT = T.MaxComponentValue();
			if (!((int)SampleFlags & (int)BxDFReflTransFlags::Reflection)) PR = 0;
			if (!((int)SampleFlags & (int)BxDFReflTransFlags::Transmission)) PT = 0;
			if (PR == 0 && PT == 0) return{};

			if(SameHemisphere(Wi, Wo))
				return PR / (PR + PT) * AbsCosTheta(Wi) * InvPi;
			else
				return PT / (PR + PT) * AbsCosTheta(Wi) * InvPi;
		}
		bool SampledPDFIsProportional()const override { return false; }
		void Regularize() { }
		BxDFFlags Flags() const
		{
			return (BxDFFlags) ((int)(R ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset) |
				(int)(T ? BxDFFlags::DiffuseTransmission : BxDFFlags::Unset));
		}
	private:
		float A, B;
		SampledSpectrum R, T;
	};

	class ConductorBxDF : public BxDFBase
	{
	public: 
		ConductorBxDF() = default;

		ConductorBxDF(const TrowbridgeReitzDistribution &mfDistrib,
			const SampledSpectrum &eta, const SampledSpectrum &k)
			: mfDistrib(mfDistrib), eta(eta), k(k) {}

		BxDFFlags Flags() const override
		{
			if (mfDistrib.EffectivelySpecular())
				return (BxDFFlags) ((int)BxDFFlags::Reflection | (int)BxDFFlags::Specular);
			else
				return (BxDFFlags)((int)BxDFFlags::Reflection | (int)BxDFFlags::Glossy);
		}

		bool SampledPDFIsProportional() const override { return false; }

		
		SampledSpectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const 
		{
			if (!SameHemisphere(wo, wi))
				return{};
			if (mfDistrib.EffectivelySpecular())
				return{};
			float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
			Vector3f wh = wi + wo;
			// Handle degenerate cases for microfacet reflection
			if (cosTheta_i == 0 || cosTheta_o == 0)
				return{};
			if (wh.X == 0 && wh.Y == 0 && wh.Z == 0)
				return{};

			Normalize(wh);
			float frCosTheta_i = AbsDot(wi, FaceForward(wh, Vector3f(0, 0, 1)));
			SampledSpectrum F = FrConductor(frCosTheta_i, eta, k);
			return F * (mfDistrib.D(wh) * mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o));
		}

		
		BSDFSample Sample_f(Vector3f wo, float uc, const Vector2f &u, TransportMode mode,
				BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
			if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
				return{};
			if (mfDistrib.EffectivelySpecular()) {
				// Compute perfect specular reflection direction
				Vector3f wi(-wo.X, -wo.Y, wo.Z);

				SampledSpectrum f = FrConductor(AbsCosTheta(wi), eta, k) / AbsCosTheta(wi);
				return BSDFSample(f, wi, 1, BxDFFlags::SpecularReflection);
			}

			// Sample microfacet orientation $\wh$ and reflected direction $\wi$
			if (wo.Z == 0)
				return{};
			Vector3f wh = mfDistrib.Sample_wm(wo, u);
			Vector3f wi = Reflect(wo, wh);
			//CHECK_RARE(1e-6, Dot(wo, wh) <= 0);
			if (!SameHemisphere(wo, wi) || Dot(wo, wh) <= 0)
				return{};

			// Compute PDF of _wi_ for microfacet reflection
			float pdf = mfDistrib.PDF(wo, wh) / (4 * Dot(wo, wh));

			// TODO: reuse fragments from f()
			float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
			// Handle degenerate cases for microfacet reflection
			if (cosTheta_i == 0 || cosTheta_o == 0)
				return{};
			float frCosTheta_i = AbsDot(wi, FaceForward(wh, Vector3f(0, 0, 1)));
			SampledSpectrum F = FrConductor(frCosTheta_i, eta, k);
			SampledSpectrum f =
				F * mfDistrib.D(wh) * mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
			return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
		}

		float PDF(Vector3f wo, Vector3f wi, TransportMode mode,BxDFReflTransFlags sampleFlags) const 
		{
			if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
				return 0;
			if (!SameHemisphere(wo, wi))
				return 0;
			if (mfDistrib.EffectivelySpecular())
				return 0;
			Vector3f wh = wo + wi;
			if (Length(wh) * Length(wh) == 0 || Dot(wo, wh) <= 0)
				return 0;
			Normalize(wh);
			return mfDistrib.PDF(wo, wh) / (4 * Dot(wo, wh));
		}

		void Regularize() { mfDistrib.Regularize(); }
	private:
		TrowbridgeReitzDistribution mfDistrib;
		SampledSpectrum eta, k;
	};
}