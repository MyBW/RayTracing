#include "BxDF.hpp"
namespace BlackWalnut
{
	SampledSpectrum BxDFBase::RHO(Vector3f Wo, std::vector<float> UC, std::vector<Vector2f> U2) const
	{
		if (Wo.Z == 0)
			return SampledSpectrum(0.f);
		SampledSpectrum r(0.);
		CHECK_EQ(UC.size(), U2.size());
		for (size_t i = 0; i < UC.size(); ++i) {
			// Estimate one term of $\rho_\roman{hd}$
			auto bs = Sample_f(Wo, UC[i], U2[i], TransportMode::Radiance);
			if (bs.Pdf != 0)
				r += bs.f * AbsCosTheta(bs.Wi) / bs.Pdf;
		}
		return r / (float)UC.size();
	}

	SampledSpectrum BxDFBase::RHO(std::vector<float> UC1, std::vector<Vector2f> U1, std::vector<float> UC2, std::vector<Vector2f> U2) const
	{
		CHECK_EQ(UC1.size(), U1.size());
		CHECK_EQ(UC2.size(), U2.size());
		CHECK_EQ(U1.size(), U2.size());
		SampledSpectrum r(0.f);
		for (size_t i = 0; i < U1.size(); ++i) {
			// Estimate one term of $\rho_\roman{hh}$
			Vector3f wo = SampleUniformHemisphere(U1[i]);
			if (wo.Z == 0)
				continue;
			float pdfo = UniformHemispherePDF();
			auto bs = Sample_f(wo, UC2[i], U2[i], TransportMode::Radiance);
			if (bs.Pdf != 0)
				r += bs.f * AbsCosTheta(bs.Wi) * AbsCosTheta(wo) / (pdfo * bs.Pdf);
		}
		return r / (Pi * U1.size());
	}
}