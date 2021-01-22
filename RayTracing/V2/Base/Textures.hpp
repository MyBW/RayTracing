#pragma once
namespace BlackWalnut
{
	struct TextureEvalContext {
		TextureEvalContext() = default;
		/*TextureEvalContext(const SurfaceInteraction &si)
			: p(si.p()),
			dpdx(si.dpdx),
			dpdy(si.dpdy),
			uv(si.uv),
			dudx(si.dudx),
			dudy(si.dudy),
			dvdx(si.dvdx),
			dvdy(si.dvdy),
			faceIndex(si.faceIndex) {}*/
		TextureEvalContext(const Vector3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
				const Point2f &uv, float dudx, float dudy, float dvdx, float dvdy,
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
	class UVMapping2D 
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
		Float su, sv, du, dv;
	};

	class SpectrumTextureBase
	{
	public:
		virtual SampledSpectrum Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const = 0;
	};
	class ImageTextureBase : public SpectrumTextureBase
	{
	public:
		ImageTextureBase(TextureMapping2D* M, const std::wstring &FileName, const std::wstring &Filter,
			float MaxAniso, WrapMode WM, float Scale, ColorEncodingBase* Encoding);
		static void ClearCache() { }
		TextureMapping2D* Mapping;
		float Scale;
		MIPmap *mipmap;
	private:
		
	};
	class SpectrumImageTexture : public ImageTextureBase {
	public:
		SpectrumImageTexture(TextureMapping2D* m, const std::wstring &filename,
			const std::wstring &filter, float maxAniso, WrapMode wm,
			float scale, ColorEncodingBase* encoding)
			: ImageTextureBase(m, filename, filter, maxAniso, wm, scale, encoding) {}

		SampledSpectrum Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const;
	};

	class FloatConstantTexture : public SpectrumTextureBase
	{
	public:
		FloatConstantTexture(float V):Value(V) {}
		float Evaluate(TextureEvalContext Ctx, SampledWavelengths lambda) const { return Value; }
	private:
		float Value;
	};
}