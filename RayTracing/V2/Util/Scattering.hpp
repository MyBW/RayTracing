#pragma once
#include "Spectrum.hpp"
#include "..\V2\Math\Math.hpp"
namespace BlackWalnut
{

	inline Vector3f Reflect(const Vector3f &wo, const Vector3f &n) 
	{
		return wo * (-1.0f) + n * Dot(wo, n)  * 2;
	}
	class TrowbridgeReitzDistribution
	{
	public:
		
		
		static inline float RoughnessToAlpha(float roughness) { return std::sqrt(roughness); }

		TrowbridgeReitzDistribution() = default;
		
		TrowbridgeReitzDistribution(float alpha_x, float alpha_y)
			: alpha_x(std::max<float>(1e-4, alpha_x)),
			alpha_y(std::max<float>(1e-4, alpha_y)) {}

		inline float D(const Vector3f &wm) const {
			float tan2Theta = Tan2Theta(wm);
			if (IsInf(tan2Theta))
				return 0;
			float cos4Theta = Cos2Theta(wm) * Cos2Theta(wm);
			if (cos4Theta < 1e-16f)
				return 0;
			float e = (Cos2Phi(wm) / Pow(alpha_x, 2) + Sin2Phi(wm) / Pow(alpha_y, 2)) * tan2Theta;
			return 1 / (Pi * alpha_x * alpha_y * cos4Theta * Pow(1 + e, 2));
		}

		
		bool EffectivelySpecular() const { return (std::min)(alpha_x, alpha_y) < 1e-3f; }

		
		float G1(const Vector3f &w) const { return 1 / (1 + Lambda(w)); }

		
		float Lambda(const Vector3f &w) const 
		{
			float tan2Theta = Tan2Theta(w);
			if (IsInf(tan2Theta))
				return 0.;
			// Compute _alpha2_ for direction _w_
			float alpha2 = Cos2Phi(w) * Pow(alpha_x,2) + Sin2Phi(w) * Pow(alpha_y,2);

			return (-1 + std::sqrt(1.f + alpha2 * tan2Theta)) / 2;
		}

		
		float G(const Vector3f &wo, const Vector3f &wi) const 
		{
			return 1 / (1 + Lambda(wo) + Lambda(wi));
		}

		
		Vector3f Sample_wm(const Vector2f &u) const 
		{
			return SampleTrowbridgeReitz(alpha_x, alpha_y, u);
		}

		
		float D(const Vector3f &w, const Vector3f &wm) const 
		{
			return D(wm) * G1(w) * std::max<float>(0, Dot(w, wm)) / AbsCosTheta(w);
		}

		
		float PDF(const Vector3f &wo, const Vector3f &wm) const 
		{
			return D(wm) * G1(wo) * AbsDot(wo, wm) / AbsCosTheta(wo);
		}

		
		Vector3f Sample_wm(const Vector3f &wo, const Vector2f &u) const 
		{
			bool flip = wo.Z < 0;
			Vector3f wm =
				SampleTrowbridgeReitzVisibleArea(flip ? wo * -1 : wo, alpha_x, alpha_y, u);
			if (flip)
				wm = wm * -1;
			return wm;
		}
		void Regularize() 
		{
			alpha_x = Clamp(2 * alpha_x, 0.1f, 0.3f);
			alpha_y = Clamp(2 * alpha_y, 0.1f, 0.3f);
		}

	private:
		// TrowbridgeReitzDistribution Private Members
		float alpha_x, alpha_y;
	};

	// https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
	
	inline SampledSpectrum FrConductor(float cosTheta_i, const SampledSpectrum &eta,
			const SampledSpectrum &k) {
		cosTheta_i = Clamp(cosTheta_i, -1, 1);
		SampledSpectrum etak = k;

		float cos2Theta_i = cosTheta_i * cosTheta_i;
		float sin2Theta_i = 1 - cos2Theta_i;
		SampledSpectrum eta2 = eta * eta;
		SampledSpectrum etak2 = etak * etak;

		SampledSpectrum t0 = eta2 - etak2 - SampledSpectrum(sin2Theta_i);
		SampledSpectrum a2plusb2 = Sqrt(t0 * t0 +  eta2 * etak2 * 4.0f);
		SampledSpectrum t1 = a2plusb2 + SampledSpectrum(cos2Theta_i);
		SampledSpectrum a = SafeSqrt((a2plusb2 + t0) * 0.5f);
		SampledSpectrum t2 =  a * 2.0f * cosTheta_i;
		SampledSpectrum Rs = (t1 - t2) / (t1 + t2);

		SampledSpectrum t3 =
			 a2plusb2 * cos2Theta_i + SampledSpectrum(sin2Theta_i * sin2Theta_i);
		SampledSpectrum t4 = t2 * sin2Theta_i;
		SampledSpectrum Rp = Rs * (t3 - t4) / (t3 + t4);

		return  (Rp + Rs) * 0.5f;
	}

}