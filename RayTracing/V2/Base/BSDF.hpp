#pragma once
#include "BxDF.hpp"
namespace BlackWalnut
{
	class BSDF
	{
	public:
		BSDF() = default;
		BSDF(const Vector3f &Wo, const Vector3f &Normal, const Vector3f &Ns, const Vector3f &DpDus, BxDFBase* InBxDF, float Eta = 1)
			:Eta(Dot(Wo, Normal) < 0 ? 1 / Eta : Eta), BxDF(InBxDF), Ng(Normal)
		{
			Vector3f Tmp = DpDus;
			Normalize(Tmp);
			ShadingFrame = Frame::FromXY(Tmp, Vector3f(Ns));
		}
		operator bool()const { return (bool)BxDF; }
		Vector3f RenderToLocal(const Vector3f &V) const { return ShadingFrame.ToLocal(V);}
		Vector3f LocalToRender(const Vector3f &V) const { return ShadingFrame.FromLocal(V); }
		BxDFBase* GetBxDF() { return BxDF; }
		void SetBxDF(BxDFBase* InBxDF) { BxDF = InBxDF; }
		bool IsNonSpecular() const { return (int)BxDF->Flags() & ((int)BxDFFlags::Diffuse | (int)BxDFFlags::Glossy); }
		bool IsDiffuse() const { return ((int)BxDF->Flags() & (int)BxDFFlags::Diffuse); }
		bool IsGlossy() const { return ((int)BxDF->Flags() & (int)BxDFFlags::Glossy); }
		bool IsSpecular() const { return ((int)BxDF->Flags() & (int)BxDFFlags::Specular); }
		bool HasReflection() const { return ((int)BxDF->Flags() & (int)BxDFFlags::Reflection); }
		bool HasTransmission() const { return ((int)BxDF->Flags() & (int)BxDFFlags::Transmission); }

		SampledSpectrum f(Vector3f woRender, Vector3f wiRender, TransportMode mode = TransportMode::Radiance) const
		{
			Vector3f Wi = RenderToLocal(wiRender);
			Vector3f Wo = RenderToLocal(woRender);
			if (Wo.Z == 0) return{};
			return BxDF->f(Wo, Wi, mode) * GBump(Wo, Wi, mode);
		}
		template<typename SpecificBxDF>
		SampledSpectrum  f(Vector3f woRender, Vector3f wiRender, TransportMode mode = TransportMode::Radiance) const
		{
			Vector3f Wi = RenderToLocal(wiRender);
			Vector3f Wo = RenderToLocal(woRender);
			if (Wo.z == 0) return{};
			const SpecificBxDF* specificBxDF = dynamic_cast<SpecificBxDF*>(BxDF);
			return specificBxDF->f(Wo, Wi, mode) * GBump(Wo, Wi, mode);
		}
		SampledSpectrum RHO(Vector3f &WoRender, std::vector<float> UC, std::vector<Vector2f> U2) const
		{
			Vector3f Wo = RenderToLocal(WoRender);
			return BxDF->RHO(Wo, UC, U2);
		}
		SampledSpectrum RHO(std::vector<float> UC1, std::vector<Vector2f> U1, std::vector<float> UC2, std::vector<Vector2f> U2)
		{
			return BxDF->RHO(UC1, U1, UC2, U2);
		}
		BSDFSample Sample_f(Vector3f WoRender, float U, const Vector2f &U2, TransportMode Mode = TransportMode::Radiance, BxDFReflTransFlags SampleFlags = BxDFReflTransFlags::All) const
		{
			Vector3f Wo = RenderToLocal(WoRender);
			if (Wo.Z == 0 || !((int)BxDF->Flags() & (int)SampleFlags))
				return{};
			BlackWalnut::BSDFSample BS = BxDF->Sample_f(Wo, U, U2, Mode, SampleFlags);
			CHECK_GE(BS.Pdf, 0);
			BS.Wi = LocalToRender(BS.Wi);
			BS.f *= GBump(WoRender, BS.Wi, Mode);
			return BS;
		}
		float PDF(Vector3f WoRender, Vector3f WiRender, TransportMode Mode = TransportMode::Radiance, BxDFReflTransFlags SampleFlags = BxDFReflTransFlags::All) const
		{
			Vector3f Wo = RenderToLocal(WoRender);
			Vector3f Wi = RenderToLocal(WiRender);
			if (Wo.Z == 0) return 0;
			return BxDF->PDF(Wo, Wi, Mode, SampleFlags);
		}
		bool SampledPDFIsProportional() const { return BxDF->SampledPDFIsProportional(); }

		template<typename SpecificBxDF>
		BSDFSample Sample_f(Vector3f WoRender, float U, const Vector2f &U2, TransportMode Mode = TransportMode::Radiance, BxDFReflTransFlags SampleFlags = BxDFReflTransFlags::All) const
		{
			Vector3f Wo = RenderToLocal(WoRender);
			if (Wo.Z == 0 || !(BxDF->Flags() & SampleFlags))
				return{};
			const SpecificBxDF* specificBxDF = dynamic_cast<SpecificBxDF*>(BxDF);
			BSDFSample BS = specificBxDF->Sample_f(Wo, U, U2, Mode, SampleFlags);
			CHECK_GE(BS.Pdf, 0);
			BS.Wi = LocalToRender(BS.wi);
			BS.f *= GBump(WoRender, BS.Wi, Mode);
			return BS;
		}
		template<typename SpecificBxDF>
		float PDF(Vector3f WoRender, Vector3f WiRender, TransportMode Mode = TransportMode::Radiance, BxDFReflTransFlags SampleFlags = BxDFReflTransFlags::All) const
		{
			Vector3f Wo = RenderToLocal(WoRender);
			Vector3f Wi = RenderToLocal(WiRender);
			if (Wo.Z == 0) return 0;
			const SpecificBxDF* specificBxDF = dynamic_cast<SpecificBxDF*>(BxDF);
			return specificBxDF->PDF(Wo, Wi, Mode, SampleFlags);
		}

		void Regularize() { BxDF->Regularize(); }
		float Eta;
	private:
		float GBump(Vector3f wo, Vector3f wi, TransportMode mode) const 
		{
			return 1;  // disable for now...

			/*Vector3f w = (mode == TransportMode::Radiance) ? wi : wo;
			Normal3f ngf = FaceForward(ng, w);
			Normal3f nsf = FaceForward(Normal3f(shadingFrame.z), ngf);
			Float cosThetaIs = std::max<Float>(0, Dot(nsf, w)), cosThetaIg = Dot(ngf, w);
			Float cosThetaN = Dot(ngf, nsf);
			CHECK_GE(cosThetaIs, 0);
			CHECK_GE(cosThetaIg, 0);
			CHECK_GE(cosThetaN, 0);

			if (cosThetaIs == 0 || cosThetaIg == 0 || cosThetaN == 0)
				return 0;
			Float G = cosThetaIg / (cosThetaIs * cosThetaN);
			if (G >= 1)
				return 1;

			return -G * G * G + G * G + G;*/
		}
		BxDFBase* BxDF;
		Frame ShadingFrame;
		Vector3f Ng;
	};
}