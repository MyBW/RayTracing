#pragma once
#include "..\V2\Math\Geom.hpp"
#include "..\V2\Util\Spectrum.hpp"
#include "..\V2\Util\Mipmap.hpp"
#include "..\V2\Interaction.hpp"
namespace BlackWalnut
{
	struct TextureEvalContext 
	{
		TextureEvalContext() = default;
		TextureEvalContext(const SurfaceInteraction &si)
			: p(si.pi),
			dpdx(si.dpdx),
			dpdy(si.dpdy),
			uv(si.uv),
			dudx(si.dudx),
			dudy(si.dudy),
			dvdx(si.dvdx),
			dvdy(si.dvdy),
			faceIndex(si.faceIndex) {}
		TextureEvalContext(const Vector3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
				const Vector2f &uv, float dudx, float dudy, float dvdx, float dvdy,
				int faceIndex)
			: p(p),
			dpdx(dpdx),
			dpdy(dpdy),
			uv(uv),
			dudx(dudx),
			dudy(dudy),
			dvdx(dvdx),
			dvdy(dvdy),
			faceIndex(faceIndex) {}

		Vector3f p;
		Vector3f dpdx, dpdy;
		Vector2f uv;
		float dudx = 0, dudy = 0, dvdx = 0, dvdy = 0;
		int faceIndex = 0;
	};
	class TextureMapping2D
	{
	public:
		virtual Vector2f Map(TextureEvalContext ctx, Vector2f *dstdx, Vector2f *dstdy) const = 0;
	};
	class UVMapping2D : public TextureMapping2D
	{
	public:
		// UVMapping2D Public Methods
		UVMapping2D(float su = 1, float sv = 1, float du = 0, float dv = 0)
			: su(su), sv(sv), du(du), dv(dv) {}
		Vector2f Map(TextureEvalContext ctx, Vector2f *dstdx, Vector2f *dstdy) const 
		{
			// Compute texture differentials for 2D $(u,v)$ mapping
			*dstdx = Vector2f(su * ctx.dudx, sv * ctx.dvdx);
			*dstdy = Vector2f(su * ctx.dudy, sv * ctx.dvdy);
			return{ su * ctx.uv[0] + du, sv * ctx.uv[1] + dv };
		}

	private:
		float su, sv, du, dv;
	};

	class SpectrumTextureBase
	{
	public:
		virtual SampledSpectrum Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const = 0;
	};


	// TexInfo Declarations
	struct TexInfo {
		TexInfo(const std::string& f, const std::string& filt, float ma, WrapMode wm,
			const ColorEncodingBase* encoding)
			: filename(f), filter(filt), maxAniso(ma), wrapMode(wm), encoding(encoding) {}
		std::string filename;
		std::string filter;
		float maxAniso;
		WrapMode wrapMode;
		const ColorEncodingBase* encoding;
		bool operator<(const TexInfo& t2) const {
			return std::tie(filename, filter, maxAniso, encoding, wrapMode) <
				std::tie(t2.filename, t2.filter, t2.maxAniso, t2.encoding, t2.wrapMode);
		}
	};
	class ImageTextureBase : public SpectrumTextureBase
	{
	public:
		ImageTextureBase(TextureMapping2D* M, const std::string &FileName, const std::string &Filter,
			float MaxAniso, WrapMode WM, float Scale, const ColorEncodingBase* Encoding);
		static void ClearCache() { }

		TextureMapping2D* Mapping;
		float Scale;
		MIPMap *mipmap;
	private:
		static MIPMap* GetTexture(const std::string& filename, const std::string& filter,
			float maxAniso, WrapMode wm, const ColorEncodingBase* encoding);

		// ImageTextureBase Private Data
		static std::map<TexInfo, std::unique_ptr<MIPMap>> textureCache;
	};
	class SpectrumImageTexture : public ImageTextureBase {
	public:
		SpectrumImageTexture(TextureMapping2D* m, const std::string &filename,
			const std::string &filter, float maxAniso, WrapMode wm,
			float scale, const ColorEncodingBase* encoding)
			: ImageTextureBase(m, filename, filter, maxAniso, wm, scale, encoding) {}

		SampledSpectrum Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
	};

	class FloatTextureBase
	{
	public:
		virtual float Evaluate(TextureEvalContext ctx) const = 0;
	};
	class FloatConstantTexture : public FloatTextureBase
	{
	public:
		FloatConstantTexture(float V):Value(V) {}
		float Evaluate(TextureEvalContext ctx) const { return Value; }
	private:
		float Value;
	};

	class SpectrumConstantTexture : public SpectrumTextureBase
	{
	public:
		SpectrumConstantTexture(BaseSpectrum* Value) :Value(Value)
		{

		}
		SampledSpectrum Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const 
		{
			return Value->Sample(lambda);
		}
	private:
		BaseSpectrum *Value;
	};

	class RGBReflectanceConstantTexture : public SpectrumTextureBase
	{
	public:
		RGBReflectanceConstantTexture(const RGBColorSpace &cs, const RGB &rgb)
			: value(cs,rgb) {}

		
		SampledSpectrum Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const 
		{
			return value.Sample(lambda);
		}
	private:
		RGBReflectanceSpectrum value;
	};


	class UniversalTextureEvaluator {
	public:

		bool CanEvaluate(std::initializer_list<FloatTextureBase*>,
			std::initializer_list<SpectrumTextureBase*>) const {
			return true;
		}

		float operator()(FloatTextureBase* tex, TextureEvalContext ctx)
		{
			return tex->Evaluate(ctx);
		}


		SampledSpectrum operator()(SpectrumTextureBase* tex, TextureEvalContext ctx,
			SampledWavelengths lambda)
		{
			return tex->Evaluate(ctx, lambda);
		}
	};
}