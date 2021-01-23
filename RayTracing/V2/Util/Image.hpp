#pragma once
#include "..\Math\Matrix.hpp"
#include "..\..\Math\BWPrimitive.h"
#include "..\Math\Geom.hpp"
#include "ColorSpace.hpp"
//#include "..\..\spdlog\spdlog\spdlog.h"
#include "float.hpp"
#include <functional>

namespace BlackWalnut
{
	enum class PixelFormat { U256, Half, Float };
	enum class WrapMode { Repeat, Black, Clamp, OctahedralSphere };
	struct WrapMode2D
	{
		WrapMode2D(WrapMode w):Wrap{w, w}{}
		WrapMode2D(WrapMode x, WrapMode y):Wrap{x, y}
		{

		}
		std::vector<WrapMode> Wrap;
	};
	// ResampleWeight Definition
	struct ResampleWeight {
		int firstTexel;
		float weight[4];
	};
	// ImageMetadata Definition
	struct ImageMetadata {
		// These may or may not be present in the metadata of an Image.
		float renderTimeSeconds;
		Matrix4X4f cameraFromWorld, NDCFromWorld;
		Bounds2i pixelBounds;
		Vector2i fullResolution;
		int samplesPerPixel;
		float MSE;
		const RGBColorSpace *colorSpace;
		std::map<std::string, std::vector<std::string>> stringVectors;

		const RGBColorSpace *GetColorSpace() const
		{
			if (colorSpace) return colorSpace;
			return RGBColorSpace::sRGB;
		}
	};
	struct ImageChannelDesc 
	{
		ImageChannelDesc()
		{
			
		}
		operator bool() const { return size() > 0; }

		size_t size() const { return offset.size(); }
		bool IsIdentity() const {
			for (size_t i = 0; i < offset.size(); ++i)
				if (offset[i] != i)
					return false;
			return true;
		}

		std::vector<int> offset;
	};
	struct ImageChannelValues
	{
		ImageChannelValues(int n, float v):Values(n,v)
		{
		}
		ImageChannelValues(std::vector<float> In):Values(In) {}
		float& operator[](int i)
		{
			CHECK(i < Values.size());
			return Values[i];
		}
		size_t size()
		{
			return Values.size();
		}
		float Average()
		{
			float Sum = 0;
			for (int i = 0 ;i < Values.size(); i++)
			{
				Sum += Values[i];
			}
			return Sum / Values.size();
		}
		std::vector<float> Values;
	};


	inline bool RemapPixelCoords(Vector2i *pp, Vector2i resolution, WrapMode2D wrapMode) 
	{
		Vector2i &p = *pp;

		if (wrapMode.Wrap[0] == WrapMode::OctahedralSphere) {
			CHECK(wrapMode.Wrap[1] == WrapMode::OctahedralSphere);
			if (p[0] < 0) {
				p[0] = -p[0];                     // mirror across u = 0
				p[1] = resolution[1] - 1 - p[1];  // mirror across v = 0.5
			}
			else if (p[0] >= resolution[0]) {
				p[0] = 2 * resolution[0] - 1 - p[0];  // mirror across u = 1
				p[1] = resolution[1] - 1 - p[1];      // mirror across v = 0.5
			}

			if (p[1] < 0) {
				p[0] = resolution[0] - 1 - p[0];  // mirror across u = 0.5
				p[1] = -p[1];                     // mirror across v = 0;
			}
			else if (p[1] >= resolution[1]) {
				p[0] = resolution[0] - 1 - p[0];      // mirror across u = 0.5
				p[1] = 2 * resolution[1] - 1 - p[1];  // mirror across v = 1
			}

			// Bleh: things don't go as expected for 1x1 images.
			if (resolution[0] == 1)
				p[0] = 0;
			if (resolution[1] == 1)
				p[1] = 0;

			return true;
		}

		for (int c = 0; c < 2; ++c) {
			if (p[c] >= 0 && p[c] < resolution[c])
				// in bounds
				continue;

			switch (wrapMode.Wrap[c]) {
			case WrapMode::Repeat:
				p[c] = Mod(p[c], resolution[c]);
				break;
			case WrapMode::Clamp:
				p[c] = Clamp(p[c], 0, resolution[c] - 1);
				break;
			case WrapMode::Black:
				return false;
			default:
				//spdlog::error("Unhandled WrapMode mode");
				CHECK(0);
			}
		}
		return true;
	}
	struct ImageAndMetadata;
	class Image
	{
	public:
		Image():Format(PixelFormat::U256){ }
		Image(PixelFormat Format, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encoding = NULL);
		Image(std::vector<uint8_t> p8, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encode);
		Image(std::vector<Half> p16, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encoding);
		Image(std::vector<float> p32, Vector2i Resolution, std::vector<std::string>& Channels, const ColorEncodingBase* Encoding = nullptr);
		
		static ImageAndMetadata Read(const std::string &filename, const ColorEncodingBase* encoding = nullptr);

		uint32_t NChannels() const { return ChannelNames.size(); }
		bool Is16Bit() const { return Format == PixelFormat::Half; }
		bool Is32Bit() const { return Format == PixelFormat::Float; }
		bool Is8Bit() const { return Format == PixelFormat::U256; }
		bool Write(const std::string &name, const ImageMetadata &metadata = {});
		ImageChannelDesc GetChannelDesc(std::vector<std::string> RequestChannel);
		Image SelectChannels(const ImageChannelDesc& Desc) const;
		float GetChannel(Vector2i P, int C, WrapMode2D warpMode = WrapMode::Clamp) const
		{
			if (!RemapPixelCoords(&P, Resolution, warpMode))
				return 0;

			switch (Format) {
			case PixelFormat::U256: {
				std::vector<float>  out(1,0);
				ColorEncoding->ToLinear({ p8[PixelOffset(P) + C]}, out);
				return out[0];
			}
			case PixelFormat::Half:
				return float(p16[PixelOffset(P) + C]);
			case PixelFormat::Float:
				return p32[PixelOffset(P) + C];
			default:
				//spdlog::error("Unhandled PixelFormat");
				CHECK(0);
				return 0;
			}
		}
		ImageChannelValues GetChannels(Vector2i P, WrapMode2D InWrapMode = WrapMode::Clamp) const;
		ImageChannelValues GetChannels(Vector2i p, const ImageChannelDesc &Desc, WrapMode2D InWrapMode = WrapMode::Clamp) const;
		void SetChannel(Vector2i p, int c, float Value)
		{
			if (IsNaN(Value))
			{
				Value = 0;
			}

			switch (Format) {
			case PixelFormat::U256:
				{
					std::vector<uint8_t>  out(1,0);
					ColorEncoding->FromLinear({Value}, out);
					p8[PixelOffset(p) + c] = out[0];
				}
				break;
			case PixelFormat::Half:
				//p16[PixelOffset(p) + c] = Half(Value);
				p16[PixelOffset(p) + c] = Half(Value);;
				break;
			case PixelFormat::Float:
				p32[PixelOffset(p) + c] = Value;
				break;
			default:
				//spdlog::error("Unhandled PixelFormat in Image::SetChannel()");
				CHECK(0);
			}
		}

		void Image::SetChannels(Vector2i p, const ImageChannelValues &values) {
			int i = 0;
			for (auto iter = values.Values.begin(); iter != values.Values.end(); ++iter, ++i)
				SetChannel(p, i, *iter);
		}
		size_t PixelOffset(Vector2i P) const
		{
			return NChannels() * (P.Y * Resolution.X + P.X);
		}
		bool WritePNG(const std::string &Name, const ImageMetadata &MetaData) const;
		void *RawPointer(Vector2i p) 
		{
			return const_cast<void *>(((const Image *)this)->RawPointer(p));
		}
		const void *RawPointer(Vector2i p) const 
		{
			if (Is8Bit())
				return p8.data() + PixelOffset(p);
			if (Is16Bit())
				return p16.data() + PixelOffset(p);
			else {
				CHECK(Is32Bit());
				return p32.data() + PixelOffset(p);
			}
		}
		Vector2i GetResolution() { return Resolution; }
		size_t BytesUsed() const { return p8.size() + 2 * p16.size() + 4 * p32.size(); }

		void CopyRectOut(const Bounds2i& Extent, std::vector<float>& buf, WrapMode2D wrapMode = WrapMode::Clamp);
		void CopyRectIn(const Bounds2i &extent, std::vector<float>& buf);

		std::vector<std::vector<float>> GetSamplingDistribution(std::function<float(Vector2f)> dxdA = [](Vector2f) { return float(1); },
			const Bounds2f &domain = Bounds2f(std::vector<float>{ 0, 0 }, std::vector<float>{1, 1}));

		float BilerpChannel(Vector2f p, int c, WrapMode2D wrapMode = WrapMode::Clamp) const {
			float x = p[0] * Resolution.X - 0.5f, y = p[1] * Resolution.Y - 0.5f;
			int xi = std::floor(x), yi = std::floor(y);
			float dx = x - xi, dy = y - yi;
			std::vector<float> v = { GetChannel({ xi, yi }, c, wrapMode),
				GetChannel({ xi + 1, yi }, c, wrapMode),
				GetChannel({ xi, yi + 1 }, c, wrapMode),
				GetChannel({ xi + 1, yi + 1 }, c, wrapMode) };
			return BlackWalnut::Bilerp({ dx, dy }, v);
		}
		ImageChannelValues Bilerp(Vector2f p, WrapMode2D wrapMode = WrapMode::Clamp) const;
		Image FloatResize(Vector2i NewResolution, WrapMode2D Wrap);
		static std::vector<Image> GenerateMIPMap(Image InImage, WrapMode2D WrapMode);
		Image ConvertToFormat(PixelFormat format, const ColorEncodingBase* encoding = nullptr);
	public:
		static std::vector<ResampleWeight> resampleWeights(int oldRes, int newRes);
		PixelFormat Format;
		Vector2i Resolution;
		const ColorEncodingBase* ColorEncoding = nullptr;
		std::vector<std::string> ChannelNames;
		std::vector<uint8_t> p8;
		std::vector<Half> p16;
		std::vector<float> p32;
		
	};
	struct ImageAndMetadata
	{
		Image image;
		ImageMetadata MetaData;
	};
}