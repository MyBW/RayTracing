#include "Textures.hpp"

namespace BlackWalnut
{

	ImageTextureBase::ImageTextureBase(TextureMapping2D* M, const std::string &FileName, const std::string &Filter, 
		float MaxAniso, WrapMode WM, float Scale, const ColorEncodingBase* Encoding)
		:Mapping(std::move(M)),Scale(Scale)
	{
		mipmap = GetTexture(FileName, Filter, MaxAniso, WM, Encoding);
	}

	MIPMap* ImageTextureBase::GetTexture(const std::string& filename, const std::string& filter, float maxAniso, WrapMode wm, const ColorEncodingBase* encoding)
	{
		// Return _MIPMap_ from texture cache if present
		TexInfo texInfo(filename, filter, maxAniso, wm, encoding);
		
		if (textureCache.find(texInfo) != textureCache.end())
			return textureCache[texInfo].get();

		// Create _MIPMap_ for _filename_
		MIPMapFilterOptions options;
		options.MaxAnisotropy = maxAniso;

		FilterFunction ff = ParseFilter(filter);
		
		options.Filter = ff;

		std::unique_ptr<MIPMap> mipmap =
			MIPMap::CreateFromFile(filename, options, wm, encoding);
		if (mipmap) {
			
			// This is actually ok, but if it hits, it means we've wastefully
			// loaded this texture. (Note that in that case, should just return
			// the one that's already in there and not replace it.)
			CHECK(textureCache.find(texInfo) == textureCache.end());
			textureCache[texInfo] = std::move(mipmap);
			return textureCache[texInfo].get();
		}
		else
			return nullptr;
	}

	std::map<BlackWalnut::TexInfo, std::unique_ptr<BlackWalnut::MIPMap>> ImageTextureBase::textureCache;

	SampledSpectrum SpectrumImageTexture::Evaluate(TextureEvalContext ctx, SampledWavelengths lambda) const
	{
		if (!mipmap) return SampledSpectrum(Scale);
		Vector2f dstdx, dstdy;
		Vector2f st = Mapping->Map(ctx, &dstdx, &dstdy);
		// Texture coordinates are (0,0) in the lower left corner, but
		// image coordinates are (0,0) in the upper left.
		st[1] = 1 - st[1];
		RGB rgb =  mipmap->Lookup<RGB>(st, dstdx, dstdy) * Scale;
		const RGBColorSpace *cs = mipmap->GetRGBColorSpace();
		if (cs != nullptr) {
			if ((std::max)({ rgb.X, rgb.Y, rgb.Z }) > 1)
				return RGBSpectrum(*cs, rgb).Sample(lambda);
			return RGBReflectanceSpectrum(*cs, rgb).Sample(lambda);
		}
		// otherwise it better be a one-channel texture
		CHECK(rgb[0] == rgb[1] && rgb[1] == rgb[2]);
		return SampledSpectrum(rgb[0]);
	}

}