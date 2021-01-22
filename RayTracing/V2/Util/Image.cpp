#include"Image.hpp"
#include "..\..\lodepng\lodepng.h"
#include <memory>
#include <stdint.h>
#include "BlueNoise.hpp"
#include "File.hpp"
namespace BlackWalnut
{

	template <typename F>
	void ForExtent(const Bounds2i &extent, WrapMode2D wrapMode, Image &image, F op) 
	{
		CHECK(extent.GetMin(0), extent.GetMax(0));
		CHECK(extent.GetMin(1), extent.GetMax(1));

		int nx = extent.Max[0] - extent.Min[0];
		int nc = image.NChannels();
		if (Intersect(extent, Bounds2i(std::vector<int>{ 0, 0 }, std::vector<int>{ image.Resolution[0], image.Resolution[1]})) == extent)
		{
			// All in bounds
			for (int y = extent.Min[1]; y < extent.Max[1]; ++y) {
				int offset = image.PixelOffset({ extent.Min[0], y });
				for (int x = 0; x < nx; ++x)
					for (int c = 0; c < nc; ++c)
						op(offset++);
			}
		}
		else {
			for (int y = extent.Min[1]; y < extent.Max[1]; ++y) {
				for (int x = 0; x < nx; ++x) {
					Vector2i p(extent.Min[0] + x, y);
					// FIXME: this will return false on Black wrap mode
					CHECK(RemapPixelCoords(&p, image.Resolution, wrapMode));
					int offset = image.PixelOffset(p);
					for (int c = 0; c < nc; ++c)
						op(offset++);
				}
			}
		}
	}


	static ImageAndMetadata ReadPNG(const std::string &name, const ColorEncodingBase *encoding);
	Image::Image(std::vector<uint8_t> p8, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encode)
		:Format(PixelFormat::U256),Resolution(Resolution),ChannelNames(Channels), ColorEncoding(Encode),p8(p8)
	{
		CHECK(p8.size() == Resolution[0] * Resolution[1] * NChannels());
	}

	Image::Image(std::vector<Half> p16, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encoding)
		:Format(PixelFormat::Half),Resolution(Resolution),ChannelNames(Channels),ColorEncoding(Encoding),p16(p16)
	{
		CHECK(p16.size() == Resolution[0] * Resolution[1] * NChannels());
		CHECK(Is16Bit());
	}

	Image::Image(std::vector<float> p32, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encoding)
		:Format(PixelFormat::Float), Resolution(Resolution), ChannelNames(Channels), ColorEncoding(Encoding), p32(p32)
	{
		CHECK(p32.size() == Resolution[0] * Resolution[1] * NChannels());
		CHECK(Is32Bit());
	}

	Image::Image(PixelFormat Format, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encoding /*= NULL*/)
		:Format(Format),Resolution(Resolution),ChannelNames(Channels), ColorEncoding(Encoding)
	{
		if (Is8Bit()) {
			p8.resize(NChannels() * Resolution[0] * Resolution[1]);
			CHECK(ColorEncoding != nullptr);
		}
		else if (Is16Bit())
			p16.resize(NChannels() * Resolution[0] * Resolution[1]);
		else if (Is32Bit())
			p32.resize(NChannels() * Resolution[0] * Resolution[1]);
		else
			//LOG_FATAL("Unhandled format in Image::Image()");
			CHECK(0);
	}

	BlackWalnut::ImageAndMetadata Image::Read(const std::string &filename, ColorEncodingBase* encoding /*= nullptr*/)
	{
		//Only Support PNG
		return ReadPNG(filename, encoding);
	}

	bool Image::Write(const std::string &name, const ImageMetadata &metadata)
	{
		
		//CHECK(metadata.pixelBounds.Area() == Resolution.X * Resolution.Y);
		if (NChannels() > 4) CHECK(0);
		const Image *image = this;
		Image RGBImage;
		if (NChannels() == 4) 
		{
			ImageChannelDesc desc = GetChannelDesc(std::vector<std::string>{ "R", "G", "B", "A" });
			if (desc) 
			{
				RGBImage = SelectChannels(GetChannelDesc(std::vector<std::string>{ "R", "G", "B" }));
				image = &RGBImage;
			}
			else 
			{
				//spdlog::error("[0]: unable to write an 4 channel image that is not RGBA.", name);
				CHECK(0);
				return false;
			}
		}
		if (NChannels() == 3 && metadata.GetColorSpace() != RGBColorSpace::sRGB)
			//spdlog::warn("[0]: writing image with non-sRGB color space to a format that  doesn't store color spaces.", name);
			CHECK(0);
		if (NChannels() == 3)
		{
			// Order as RGB
			ImageChannelDesc desc = GetChannelDesc(std::vector<std::string>{ "R", "G", "B" });
			if (!desc)
				//CHECK(0);spdlog::warn("[0]: 3-channels but doesn't have R, G, and B. Image may be garbled.", name);
				CHECK(0);
			else 
			{
				RGBImage = SelectChannels(desc);
				image = &RGBImage;
			}
		}

		//Only Support PNG suffix
		return image->WritePNG(name, metadata);
	}

	ImageChannelDesc Image::GetChannelDesc(std::vector<std::string> RequestChannel)
	{
		ImageChannelDesc Desc;
		Desc.offset.resize(RequestChannel.size());
		for (size_t i = 0; i < RequestChannel.size(); i++)
		{
			size_t j;
			for (j = 0; j < ChannelNames.size(); ++j)
				if (RequestChannel[i] == ChannelNames[j]) 
				{
					Desc.offset[i] = j;
					break;
				}
			if (j == ChannelNames.size())
				return{};
		}
		return Desc;
	}

	Image Image::SelectChannels(const ImageChannelDesc& Desc) const
	{
		std::vector<std::string> descChannelNames;
		// TODO: descChannelNames = ChannelNames(desc)
		for (size_t i = 0; i < Desc.offset.size(); ++i)
			descChannelNames.push_back(ChannelNames[Desc.offset[i]]);

		Image image(Format, Resolution, descChannelNames, ColorEncoding);
		for (int y = 0; y < Resolution.Y; ++y)
			for (int x = 0; x < Resolution.X; ++x)
				image.SetChannels({ x, y }, GetChannels({ x, y }, Desc));
		return image;
	}

	ImageChannelValues Image::GetChannels(Vector2i P, WrapMode2D InWrapMode /*= WrapMode::Clamp*/) const
	{
		ImageChannelValues CV(NChannels(), 0.0f);
		if (!RemapPixelCoords(&P, Resolution, InWrapMode))
		{
			return CV;
		}
		size_t pixelOffset = PixelOffset(P);
		switch (Format) {
		case PixelFormat::U256: 
		{
			for (int i = 0; i < NChannels(); i++)
			{
				std::vector<float> Out(1, 0);
				ColorEncoding->ToLinear({ p8[pixelOffset + i]}, Out);
				CV[i] = Out[0];
			}
			
			break;
		}
		case PixelFormat::Half: 
		{
			for (int i = 0; i < NChannels(); ++i)
				CV[i] = float(p16[pixelOffset + i]);
			break;
		}
		case PixelFormat::Float: 
		{
			for (int i = 0; i < NChannels(); ++i)
				CV[i] = p32[pixelOffset + i];
			break;
		}
		default:
			//spdlog::error("Unhandled PixelFormat");
			CHECK(0);
		}

		return CV;
	}

	ImageChannelValues Image::GetChannels(Vector2i p, const ImageChannelDesc &desc, WrapMode2D InWrapMode /*= WrapMode::Clamp*/) const
	{
		ImageChannelValues CV(desc.offset.size(), float(0));
		if (!RemapPixelCoords(&p, Resolution, InWrapMode))
			return CV;

		size_t pixelOffset = PixelOffset(p);

		switch (Format) {
		case PixelFormat::U256: {
			for (int i = 0; i < desc.offset.size(); ++i)
			{
				std::vector<float> Out(1, 0);
				ColorEncoding->ToLinear({ p8[pixelOffset + desc.offset[i]] }, Out);
				CV[i] = Out[0];
			}
				
			break;
		}
		case PixelFormat::Half: {
			for (int i = 0; i < desc.offset.size(); ++i)
				CV[i] = float(p16[pixelOffset + desc.offset[i]]);
			break;
		}
		case PixelFormat::Float: {
			for (int i = 0; i < desc.offset.size(); ++i)
				CV[i] = p32[pixelOffset + desc.offset[i]];
			break;
		}
		default:
			//spdlog::error("Unhandled PixelFormat");
			CHECK(0);
		}
		return CV;
	}

	bool Image::WritePNG(const std::string &Name, const ImageMetadata &MetaData) const
	{
		unsigned int error = 0;
		int nOutOfGamut = 0;

		if (Format == PixelFormat::U256) {
			if (NChannels() == 1)
				error = lodepng_encode_file(Name.c_str(), p8.data(), Resolution.X,
					Resolution.Y, LCT_GREY, 8 /* bitdepth */);
			else if (NChannels() == 3)
				// TODO: it would be nice to store the color encoding used in the
				// PNG metadata...
				error = lodepng_encode24_file(Name.c_str(), p8.data(), Resolution.X,
					Resolution.Y);
			else
				//spdlog::error("Unhandled channel count in WritePNG(): [0]", NChannels());
				CHECK(0);
		}
		else if (NChannels() == 3) {
			// It may not actually be RGB, but that's what PNG's going to
			// assume..
			std::unique_ptr<uint8_t[]> rgb8 =
				std::make_unique<uint8_t[]>(3 * Resolution.X * Resolution.Y);
			for (int y = 0; y < Resolution.Y; ++y)
				for (int x = 0; x < Resolution.X; ++x)
					for (int c = 0; c < 3; ++c) {
						float dither = -.5f + BlueNoise(c, { x, y });
						float v = GetChannel({ x, y }, c);
						if (v < 0 || v > 1)
							++nOutOfGamut;
						rgb8[3 * (y * Resolution.X + x) + c] = LinearToSRGB8(v, dither);
					}

			error =
				lodepng_encode24_file(Name.c_str(), rgb8.get(), Resolution.X, Resolution.Y);
		}
		else if (NChannels() == 1) {
			std::unique_ptr<uint8_t[]> y8 =
				std::make_unique<uint8_t[]>(Resolution.X * Resolution.Y);
			for (int y = 0; y < Resolution.Y; ++y)
				for (int x = 0; x < Resolution.X; ++x) {
					float dither = -.5f + BlueNoise(0, { x, y });
					float v = GetChannel({ x, y }, 0);
					if (v < 0 || v > 1)
						++nOutOfGamut;
					y8[y * Resolution.X + x] = LinearToSRGB8(v, dither);
				}

			error = lodepng_encode_file(Name.c_str(), y8.get(), Resolution.X, Resolution.Y,
				LCT_GREY, 8 /* bitdepth */);
		}

		if (nOutOfGamut > 0)
			/*spdlog::warn("[0]: [1] out of gamut pixel channels clamped to [0,1].", name,
				nOutOfGamut);*/
			WARN("nOutOfGamut");

		if (error != 0) {
			//spdlog::error("Error writing PNG [0] : [1].", name, lodepng_error_text(error));
			CHECK(0);
			return false;
		}
		return true;
	}

	void Image::CopyRectOut(const Bounds2i& Extent, std::vector<float>& buf, WrapMode2D wrapMode /*= WrapMode::Clamp*/)
	{
		CHECK(buf.size() >= Extent.Area() * NChannels());

		auto bufIter = buf.begin();
		switch (Format) {
		case PixelFormat::U256:
			if (Intersect(Extent, Bounds2i( std::vector<int>{ 0, 0 }, std::vector<int>{Resolution[0],Resolution[1]})) == Extent)
			{
				// All in bounds
				size_t count = NChannels() * (Extent.GetMax(0) - Extent.GetMin(0));
				for (int y = Extent.GetMin(1); y < Extent.GetMax(1); ++y) {
					// Convert scanlines all at once.
					size_t offset = PixelOffset({ Extent.GetMin(0), y });
					for (int i = 0; i < count; ++i) 
					{
						std::vector<float> Out(1, 0);
						ColorEncoding->ToLinear({ p8[offset + i]}, Out);
						*bufIter++ = Out[0];
					}
					bufIter += count;
				}
			}
			else {
				ForExtent(Extent, wrapMode, *this, [&bufIter, this](int offset) {

					std::vector<float> Out(1, 0);
					ColorEncoding->ToLinear({p8[offset]}, Out);
					*bufIter = Out[0];
					++bufIter;
				});
			}
			break;

		case PixelFormat::Half:
			ForExtent(Extent, wrapMode, *this,
				[&bufIter, this](int offset) { *bufIter++ = float(p16[offset]); });
			break;

		case PixelFormat::Float:
			ForExtent(Extent, wrapMode, *this,
				[&bufIter, this](int offset) { *bufIter++ = float(p32[offset]); });
			break;

		default:
			CHECK(0);
		}
	}

	void Image::CopyRectIn(const Bounds2i &extent, std::vector<float>& buf)
	{
		CHECK(buf.size() >= extent.Area() * NChannels());

		auto bufIter = buf.begin();
		switch (Format) {
		case PixelFormat::U256:
			if (Intersect(extent, Bounds2i(std::vector<int>{ 0, 0 }, std::vector<int>{Resolution[0], Resolution[1]})) == extent) {
				// All in bounds
				size_t count = NChannels() * (extent.GetMax(0) - extent.GetMin(0));
				for (int y = extent.GetMin(1); y < extent.GetMax(1); ++y) {
					// Convert scanlines all at once.
					size_t offset = PixelOffset({ extent.GetMin(0), y });

					for (int i = 0; i < count; ++i) 
					{
						float v = *bufIter++;
						std::vector<uint8_t> Out(1,0);
						ColorEncoding->FromLinear({v}, Out);
						p8[offset + i] = Out[0];
					}
				}
			}
			else
				ForExtent(extent, WrapMode::Clamp, *this, [&bufIter, this](int offset) {
				float v = *bufIter++;
				std::vector<uint8_t> Out(1, 0);
				ColorEncoding->FromLinear({v}, Out);
				p8[offset] = Out[0];
			});
			break;

		case PixelFormat::Half:
			ForExtent(extent, WrapMode::Clamp, *this,
				[&bufIter, this](int offset) { p16[offset] = Half(*bufIter++); });
			break;

		case PixelFormat::Float:
			ForExtent(extent, WrapMode::Clamp, *this,
				[&bufIter, this](int offset) { p32[offset] = *bufIter++; });
			break;

		default:
			CHECK(0);
		}
	}

	std::vector<std::vector<float>> Image::GetSamplingDistribution(std::function<float(Vector2f)> dxdA, const Bounds2f &domain)
	{
		std::vector<std::vector<float>> dist;
		dist.resize(Resolution[0]);
		for (int i = 0; i < dist.size(); i++)
		{
			dist[i].resize(Resolution[1]);
		}
		for (int y = 0; y < Resolution[1]; y++)
		{
			for (int x = 0; x < Resolution[0]; ++x) {
				float value = GetChannels({ x, y }).Average();

				// Assume jacobian term is basically constant over the
				// region.
				std::vector<float> p = domain.Lerp(std::vector<float>{(x + .5f) / Resolution[0], (y + .5f) / Resolution[1]});

				dist[x][y] = value * dxdA( Vector2f(p[0], p[1]));
			}
		}
		return dist;
	}

	static ImageAndMetadata ReadPNG(const std::string &name, const ColorEncodingBase *encoding)
	{
		std::string contents = ReadFileContents(name);

		if (encoding == nullptr)
			encoding = ColorEncodingBase::sRGB;

		unsigned width, height;
		LodePNGState state;
		lodepng_state_init(&state);
		unsigned int error = lodepng_inspect(
			&width, &height, &state, (const unsigned char *)contents.data(), contents.size());
		if (error != 0)
			//spdlog::error("[0]: [1]", name, lodepng_error_text(error));
			CHECK(0);

		Image image;
		switch (state.info_png.color.colortype) {
		case LCT_GREY:
		case LCT_GREY_ALPHA: {
			std::vector<unsigned char> buf;
			int bpp = state.info_png.color.bitdepth == 16 ? 16 : 8;
			error =
				lodepng::decode(buf, width, height, (const unsigned char *)contents.data(),
					contents.size(), LCT_GREY, bpp);
			if (error != 0)
				//spdlog::error("[0]: [1]", name, lodepng_error_text(error));
				CHECK(0);

			if (state.info_png.color.bitdepth == 16) {
				image = Image(PixelFormat::Half, Vector2i(width, height), std::vector<std::string>{ "Y" });
				auto bufIter = buf.begin();
				for (unsigned int y = 0; y < height; ++y)
					for (unsigned int x = 0; x < width; ++x, bufIter += 2) {
						// Convert from little endian.
						float v = (((int)bufIter[0] << 8) + (int)bufIter[1]) / 65535.f;
						v = encoding->ToFloatLinear(v);
						image.SetChannel(Vector2i(x, y), 0, v);
					}
				CHECK(bufIter == buf.end());
			}
			else {
				image = Image(PixelFormat::U256, Vector2i(width, height), std::vector<std::string>{ "Y" }, encoding);
				std::copy(buf.begin(), buf.end(), (uint8_t *)image.RawPointer({ 0, 0 }));
			}
			return ImageAndMetadata{ image, ImageMetadata() };
		}
		default: {
			std::vector<unsigned char> buf;
			int bpp = state.info_png.color.bitdepth == 16 ? 16 : 8;
			bool hasAlpha = (state.info_png.color.colortype == LCT_RGBA);
			// Force RGB if it's palletted or whatever.
			error =
				lodepng::decode(buf, width, height, (const unsigned char *)contents.data(),
					contents.size(), hasAlpha ? LCT_RGBA : LCT_RGB, bpp);
			if (error != 0)
				//spdlog::error("[0]: [1]", name, lodepng_error_text(error));
				CHECK(0);

			ImageMetadata metadata;
			metadata.colorSpace = RGBColorSpace::sRGB;
			if (state.info_png.color.bitdepth == 16) {
				if (hasAlpha) {
					image = Image(PixelFormat::Half, Vector2i(width, height), std::vector<std::string>{ "R", "G", "B", "A" });
					auto bufIter = buf.begin();
					for (unsigned int y = 0; y < height; ++y)
						for (unsigned int x = 0; x < width; ++x, bufIter += 8) {
							CHECK(bufIter < buf.end());
							// Convert from little endian.
							float rgba[4] = {
								(((int)bufIter[0] << 8) + (int)bufIter[1]) / 65535.f,
								(((int)bufIter[2] << 8) + (int)bufIter[3]) / 65535.f,
								(((int)bufIter[4] << 8) + (int)bufIter[5]) / 65535.f,
								(((int)bufIter[6] << 8) + (int)bufIter[7]) / 65535.f };
							for (int c = 0; c < 4; ++c) {
								rgba[c] = encoding->ToFloatLinear(rgba[c]);
								image.SetChannel(Vector2i(x, y), c, rgba[c]);
							}
						}
					CHECK(bufIter == buf.end());
				}
				else {
					image = Image(PixelFormat::Half, Vector2i(width, height), std::vector<std::string>{ "R", "G", "B" });
					auto bufIter = buf.begin();
					for (unsigned int y = 0; y < height; ++y)
						for (unsigned int x = 0; x < width; ++x, bufIter += 6) {
							CHECK(bufIter < buf.end());
							// Convert from little endian.
							float rgb[3] = {
								(((int)bufIter[0] << 8) + (int)bufIter[1]) / 65535.f,
								(((int)bufIter[2] << 8) + (int)bufIter[3]) / 65535.f,
								(((int)bufIter[4] << 8) + (int)bufIter[5]) / 65535.f };
							for (int c = 0; c < 3; ++c) {
								rgb[c] = encoding->ToFloatLinear(rgb[c]);
								image.SetChannel(Vector2i(x, y), c, rgb[c]);
							}
						}
					CHECK(bufIter == buf.end());
				}
			}
			else if (hasAlpha) {
				image = Image(PixelFormat::U256, Vector2i(width, height), std::vector<std::string>{ "R", "G", "B", "A" },
					encoding);
				std::copy(buf.begin(), buf.end(), (uint8_t *)image.RawPointer({ 0, 0 }));
			}
			else {
				image = Image(PixelFormat::U256, Vector2i(width, height), std::vector<std::string>{ "R", "G", "B" },
					encoding);
				std::copy(buf.begin(), buf.end(), (uint8_t *)image.RawPointer({ 0, 0 }));
			}
			return ImageAndMetadata{ image, metadata };
		}
		}
	}
}


