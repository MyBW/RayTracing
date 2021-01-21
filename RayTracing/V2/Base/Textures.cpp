#include "Textures.hpp"

namespace BlackWalnut
{

	ImageTextureBase::ImageTextureBase(TextureMapping2D* M, const std::wstring &FileName, const std::wstring &Filter, float MaxAniso, WrapMode WM, float Scale, ColorEncodingBase* Encoding)
		:Mapping(std::move(M)),Scale(Scale)
	{

	}

	SampledSpectrum SpectrumImageTexture::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const
	{
		if (!mipmap) return SampledSpectrum(Scale);
		Vector2f dstdx, dstdy;
		Vector2f st = mapping.Map(ctx, &dstdx, &dstdy);
		// Texture coordinates are (0,0) in the lower left corner, but
		// image coordinates are (0,0) in the upper left.
		st[1] = 1 - st[1];
		RGB rgb = scale * mipmap->Lookup<RGB>(st, dstdx, dstdy);
		const RGBColorSpace *cs = mipmap->GetRGBColorSpace();
		if (cs != nullptr) {
			if ((std::max)({ rgb.r, rgb.g, rgb.b }) > 1)
				return RGBSpectrum(*cs, rgb).Sample(lambda);
			return RGBReflectanceSpectrum(*cs, rgb).Sample(lambda);
		}
		// otherwise it better be a one-channel texture
		CHECK(rgb[0] == rgb[1] && rgb[1] == rgb[2]);
		return SampledSpectrum(rgb[0]);
	}

}