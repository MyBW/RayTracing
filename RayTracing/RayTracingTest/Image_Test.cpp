#include"gtest/gtest.h"
#include "..\V2\Util\Image.hpp"
using namespace BlackWalnut;
TEST(Image, Basics) {
	const ColorEncodingBase* encoding = ColorEncodingBase::Linear;
	Image y8( PixelFormat::U256, Vector2i(4, 8), std::vector<std::string>{ "Y" }, encoding);
	EXPECT_EQ(y8.NChannels(), 1);
	EXPECT_EQ(y8.BytesUsed(), y8.GetResolution()[0] * y8.GetResolution()[1]);

	Image y16(PixelFormat::Half, { 4, 8 }, std::vector<std::string>{ "Y" });
	EXPECT_EQ(y16.NChannels(), 1);
	EXPECT_EQ(y16.BytesUsed(), 2 * y16.GetResolution()[0] * y16.GetResolution()[1]);

	Image y32(PixelFormat::Float, { 4, 8 }, std::vector<std::string>{ "Y" });
	EXPECT_EQ(y32.NChannels(), 1);
	EXPECT_EQ(y32.BytesUsed(), 4 * y32.GetResolution()[0] * y32.GetResolution()[1]);

	Image rgb8(PixelFormat::U256, { 4, 8 }, std::vector<std::string>{ "R", "G", "B" }, encoding);
	EXPECT_EQ(rgb8.NChannels(), 3);
	EXPECT_EQ(rgb8.BytesUsed(), 3 * rgb8.GetResolution()[0] * rgb8.GetResolution()[1]);

	Image rgb16(PixelFormat::Half, { 4, 16 }, std::vector<std::string>{ "R", "G", "B" });
	EXPECT_EQ(rgb16.NChannels(), 3);
	EXPECT_EQ(rgb16.BytesUsed(), 2 * 3 * rgb16.GetResolution()[0] * rgb16.GetResolution()[1]);

	Image rgb32(PixelFormat::Float, { 4, 32 }, std::vector<std::string>{ "R", "G", "B" });
	EXPECT_EQ(rgb32.NChannels(), 3);
	EXPECT_EQ(rgb32.BytesUsed(), 4 * 3 * rgb32.GetResolution()[0] * rgb32.GetResolution()[1]);
}
static std::vector<float> GetFloatPixels(Vector2i res, int nc) {
	std::vector<float> p;
	for (int y = 0; y < res[1]; ++y)
		for (int x = 0; x < res[0]; ++x)
			for (int c = 0; c < nc; ++c)
				if (c == 0)
					p.push_back(std::sin(x / 4.) * std::cos(y / 8.));
				else
					p.push_back(-.25 +
						2. * (c + 3 * x + 3 * y * res[0]) / (res[0] * res[1]));
	return p;
}

static float modelQuantization(float value, PixelFormat format) {
	switch (format) {
	case PixelFormat::U256:
		return Clamp((value * 255.f) + 0.5f, 0.0f, 255.0f) * (1.f / 255.f);
	case PixelFormat::Half:
		return float(Half(value));
	case PixelFormat::Float:
		return value;
	default:
		//LOG_FATAL("Unhandled pixel format");
		CHECK(0);
	}
}


static float sRGBRoundTrip(float v, float dither = 0) {
	if (v < 0)
		return 0;
	else if (v > 1)
		return 1;
	uint8_t encoded = LinearToSRGB8(v, dither);
	return SRGB8ToLinear(encoded);
}

TEST(Image, GetSetY) {
	Vector2i res(9, 3);
	std::vector<float> yPixels = GetFloatPixels(res, 1);

	for (auto format : { PixelFormat::U256, PixelFormat::Half, PixelFormat::Float }) 
	{
		Image image(format, res,  std::vector<std::string>{ "Y" }, ColorEncodingBase::Linear);
		for (int y = 0; y < res[1]; ++y)
			for (int x = 0; x < res[0]; ++x) {
				image.SetChannel({ x, y }, 0, yPixels[y * res[0] + x]);
			}

		ImageChannelDesc yDesc = image.GetChannelDesc({ "Y" });
		EXPECT_TRUE(bool(yDesc));
		EXPECT_FALSE(bool(image.GetChannelDesc({ "Y0L0" })));

		for (int y = 0; y < res[1]; ++y)
			for (int x = 0; x < res[0]; ++x) {
				float v = image.GetChannel({ x, y }, 0);
				ImageChannelValues cv = image.GetChannels({ x, y });
				EXPECT_EQ(1, cv.size());
				EXPECT_EQ(v, cv[0]);

				cv = image.GetChannels({ x, y }, yDesc);
				EXPECT_EQ(1, cv.size());
				EXPECT_EQ(v, cv[0]);

				if (format == PixelFormat::U256)
					EXPECT_LT(std::abs(v - Clamp(yPixels[y * res[0] + x], 0.0f, 1.0f)),
						0.501f / 255.f)
					<< v << " vs " << Clamp(yPixels[y * res[0] + x], 0.0f, 1.0f);
				else
					EXPECT_EQ(v, modelQuantization(yPixels[y * res[0] + x], format));
			}
	}
}
TEST(Image, GetSetRGB) {
	Vector2i res(7, 32);
	std::vector<float> rgbPixels = GetFloatPixels(res, 3);

	for (auto format : { PixelFormat::U256, PixelFormat::Half, PixelFormat::Float }) {
		Image image(format, res, std::vector<std::string>{ "R", "G", "B" }, ColorEncodingBase::Linear);
		for (int y = 0; y < res[1]; ++y)
			for (int x = 0; x < res[0]; ++x)
				for (int c = 0; c < 3; ++c)
					image.SetChannel({ x, y }, c, rgbPixels[3 * y * res[0] + 3 * x + c]);

		ImageChannelDesc rgbDesc = image.GetChannelDesc({ "R", "G", "B" });
		EXPECT_TRUE(bool(rgbDesc));
		EXPECT_FALSE(bool(image.GetChannelDesc({ "R", "Gxxx", "B" })));

		for (int y = 0; y < res[1]; ++y)
			for (int x = 0; x < res[0]; ++x) {
				ImageChannelValues rgb = image.GetChannels({ x, y });
				EXPECT_EQ(3, rgb.size());

				ImageChannelValues rgb2 = image.GetChannels({ x, y }, rgbDesc);
				EXPECT_EQ(rgb[0], rgb2[0]);
				EXPECT_EQ(rgb[1], rgb2[1]);
				EXPECT_EQ(rgb[2], rgb2[2]);

				for (int c = 0; c < 3; ++c) {
					EXPECT_EQ(rgb[c], image.GetChannel({ x, y }, c));
					int offset = 3 * y * res[0] + 3 * x + c;
					if (format == PixelFormat::U256)
						EXPECT_LT(std::abs(rgb[c] - Clamp(rgbPixels[offset], 0.0f, 1.0f)),
							0.501f / 255.f);
					else {
						float qv = modelQuantization(rgbPixels[offset], format);
						EXPECT_EQ(rgb[c], qv);
					}
				}
			}
	}
}

TEST(Image, GetSetBGR) {
	Vector2i res(7, 32);
	std::vector<float> rgbPixels = GetFloatPixels(res, 3);

	for (auto format : { PixelFormat::U256, PixelFormat::Half, PixelFormat::Float }) {
		Image image(format, res, std::vector<std::string>{ "R", "G", "B" }, ColorEncodingBase::Linear);
		for (int y = 0; y < res[1]; ++y)
			for (int x = 0; x < res[0]; ++x)
				for (int c = 0; c < 3; ++c)
					image.SetChannel({ x, y }, c, rgbPixels[3 * y * res[0] + 3 * x + c]);

		ImageChannelDesc bgrDesc = image.GetChannelDesc({ "B", "G", "R" });
		EXPECT_TRUE(bool(bgrDesc));

		for (int y = 0; y < res[1]; ++y)
			for (int x = 0; x < res[0]; ++x) {
				ImageChannelValues rgb = image.GetChannels({ x, y });
				EXPECT_EQ(3, rgb.size());

				ImageChannelValues bgr = image.GetChannels({ x, y }, bgrDesc);
				EXPECT_EQ(rgb[0], bgr[2]);
				EXPECT_EQ(rgb[1], bgr[1]);
				EXPECT_EQ(rgb[2], bgr[0]);

				for (int c = 0; c < 3; ++c) {
					EXPECT_EQ(rgb[c], image.GetChannel({ x, y }, c));
					int offset = 3 * y * res[0] + 3 * x + c;
					if (format == PixelFormat::U256)
						EXPECT_LT(std::abs(rgb[c] - Clamp(rgbPixels[offset], 0.0f, 1.0f)),
							0.501f / 255.f);
					else {
						float qv = modelQuantization(rgbPixels[offset], format);
						EXPECT_EQ(rgb[c], qv);
					}
				}
			}
	}
}
TEST(Image, CopyRectOut) {
	Vector2i res(29, 14);

	for (auto format : { PixelFormat::U256, PixelFormat::Half, PixelFormat::Float }) {
		// for (int nc : { 1, 3 }) {
		for (int nc = 1; nc < 4; ++nc) {
			std::vector<float> orig = GetFloatPixels(res, nc);

			std::vector<std::string> channelNames = { "A" };
			for (int i = 1; i < nc; ++i)
				channelNames.push_back(std::string(1, 'A' + 1));

			Image image(format, res, channelNames, ColorEncodingBase::Linear);

			auto origIter = orig.begin();
			for (int y = 0; y < res[1]; ++y)
				for (int x = 0; x < res[0]; ++x)
					for (int c = 0; c < nc; ++c, ++origIter)
						image.SetChannel({ x, y }, c, *origIter);

			Bounds2i extent(std::vector<int>{2, 3}, std::vector<int>{5, 10});
			std::vector<float> buf(extent.Area() * nc);

			image.CopyRectOut(extent, buf);

			// Iterate through the points in the extent and the buffer
			// together.
			auto bufIter = buf.begin();

			extent.Itorator([&bufIter, nc, &image, &buf](int x, int y)
			{
				for (int c = 0; c < nc; c++)
				{
					ASSERT_FALSE(bufIter == buf.end());
					EXPECT_EQ(*bufIter, image.GetChannel(Vector2i(x, y), c));
					++bufIter;
				}
				
			}
			);

			/*for (auto pIter = begin(extent); pIter != end(extent); ++pIter) {
				for (int c = 0; c < nc; ++c) {
					ASSERT_FALSE(bufIter == buf.end());
					EXPECT_EQ(*bufIter, image.GetChannel(*pIter, c));
					++bufIter;
				}
			}*/
		}
	}
}

TEST(Image, CopyRectIn) {
	Vector2i res(17, 32);
	RNG rng;

	for (auto format : { PixelFormat::U256, PixelFormat::Half, PixelFormat::Float }) {
		for (int nc = 1; nc < 4; ++nc) {
			std::vector<float> orig = GetFloatPixels(res, nc);

			std::vector<std::string> channelNames = { "A" };
			for (int i = 1; i < nc; ++i)
				channelNames.push_back(std::string(1, 'A' + 1));

			Image image(format, res, channelNames, ColorEncodingBase::Linear);
			auto origIter = orig.begin();
			for (int y = 0; y < res[1]; ++y)
				for (int x = 0; x < res[0]; ++x)
					for (int c = 0; c < nc; ++c, ++origIter)
						image.SetChannel({ x, y }, c, *origIter);

			Bounds2i extent(std::vector<int>{10, 23}, std::vector<int>{17, 28});
			std::vector<float> buf(extent.Area() * nc);
			std::generate(buf.begin(), buf.end(),
				[&rng]() { return rng.Uniform<float>(); });

			image.CopyRectIn(extent, buf);

			// Iterate through the points in the extent and the buffer
			// together.
			auto bufIter = buf.begin();

			extent.Itorator([&bufIter, nc, format, &buf, &image](int X, int Y)
			{
				for (int c = 0; c < nc; ++c) {
					ASSERT_FALSE(bufIter == buf.end());
					if (format == PixelFormat::U256) {
						float err =
							std::abs(image.GetChannel(Vector2i(X,Y), c) - Clamp(*bufIter, 0.0f, 1.0f));
						EXPECT_LT(err, 0.501f / 255.f);
					}
					else {
						float qv = modelQuantization(*bufIter, format);
						EXPECT_EQ(qv, image.GetChannel(Vector2i(X,Y), c));
					}
					++bufIter;
				}
			}
			);
			/*for (auto pIter = begin(extent); pIter != end(extent); ++pIter) {
				for (int c = 0; c < nc; ++c) {
					ASSERT_FALSE(bufIter == buf.end());
					if (format == PixelFormat::U256) {
						float err =
							std::abs(image.GetChannel(*pIter, c) - Clamp(*bufIter, 0, 1));
						EXPECT_LT(err, 0.501f / 255.f);
					}
					else {
						float qv = modelQuantization(*bufIter, format);
						EXPECT_EQ(qv, image.GetChannel(*pIter, c));
					}
					++bufIter;
				}
			}*/
		}
	}
}

TEST(Image, PngRgbIO) {
	Vector2i res(11, 50);
	std::vector<float> rgbPixels = GetFloatPixels(res, 3);

	Image image(rgbPixels, res, std::vector<std::string>{ "R", "G", "B" });
	EXPECT_TRUE(image.Write("test.png"));
	ImageAndMetadata read = Image::Read("test.png");

	EXPECT_EQ(image.Resolution, read.image.Resolution);
	EXPECT_EQ(read.image.Format, PixelFormat::U256);
	ASSERT_TRUE(read.image.ColorEncoding != nullptr);
	// EXPECT_EQ(*read.image.Encoding(), *ColorEncodingHandle::sRGB);
	ASSERT_TRUE((bool)read.MetaData.colorSpace);
	EXPECT_EQ(RGBColorSpace::sRGB, read.MetaData.GetColorSpace());

	for (int y = 0; y < res[1]; ++y)
		for (int x = 0; x < res[0]; ++x)
			for (int c = 0; c < 3; ++c) {
				EXPECT_LE(sRGBRoundTrip(image.GetChannel({ x, y }, c), -.5f),
					read.image.GetChannel({ x, y }, c))
					<< " x " << x << ", y " << y << ", c " << c << ", orig "
					<< rgbPixels[3 * y * res[0] + 3 * x + c];
				EXPECT_LE(read.image.GetChannel({ x, y }, c),
					sRGBRoundTrip(image.GetChannel({ x, y }, c), 0.5f))
					<< " x " << x << ", y " << y << ", c " << c << ", orig "
					<< rgbPixels[3 * y * res[0] + 3 * x + c];
			}

	EXPECT_EQ(0, remove("test.png"));
}



//TEST(Image, SampleSimple) {
//	std::vector<float> texels = { float(0), float(1), float(0), float(0) };
//	Image zeroOne(texels, { 2, 2 }, std::vector<std::string>{ "Y" });
//	PiecewiseConstant2D distrib(zeroOne.GetSamplingDistribution());
//	RNG rng;
//	for (int i = 0; i < 1000; ++i) {
//		Point2f u(rng.Uniform<float>(), rng.Uniform<float>());
//		float pdf;
//		Point2f p = distrib.Sample(u, &pdf);
//		// Due to bilerp on lookup, the non-zero range goes out a bit.
//		EXPECT_GE(p.x, 0.5);
//		EXPECT_LE(p.y, 0.5);
//	}
//}

//TEST(Image, SampleLinear) {
//	int w = 500, h = 500;
//	std::vector<float> v;
//	for (int y = 0; y < h; ++y) {
//		float fy = (y + .5) / h;
//		for (int x = 0; x < w; ++x) {
//			float fx = (x + .5) / w;
//			// This integrates to 1 over [0,1]^2
//			float f = fx + fy;
//			v.push_back(f);
//		}
//	}
//
//	Image image(v, { w, h }, std::vector<std::string>{ "Y" });
//	PiecewiseConstant2D distrib(image.GetSamplingDistribution());
//	RNG rng;
//	for (int i = 0; i < 1000; ++i) {
//		Point2f u(rng.Uniform<float>(), rng.Uniform<float>());
//		float pdf;
//		Point2f p = distrib.Sample(u, &pdf);
//		float f = p.x + p.y;
//		// Allow some error since PiecewiseConstant2D uses a piecewise constant
//		// sampling distribution.
//		EXPECT_LE(std::abs(f - pdf), 3e-3) << u << ", f: " << f << ", pdf: " << pdf;
//	}
//}

//TEST(Image, SampleSinCos) {
//	int w = 500, h = 500;
//	auto f = [](Point2f p) {
//		return std::abs(std::sin(3. * p.x) * Sqr(std::cos(4. * p.y)));
//	};
//	// Integral of f over [0,1]^2
//	float integral = 1. / 24. * Sqr(std::sin(1.5)) * (8 + std::sin(8.));
//
//	std::vector<float> v;
//	for (int y = 0; y < h; ++y) {
//		float fy = (y + .5) / h;
//		for (int x = 0; x < w; ++x) {
//			float fx = (x + .5) / w;
//			v.push_back(f({ fx, fy }));
//		}
//	}
//
//	Image image(v, { w, h }, { "Y" });
//	PiecewiseConstant2D distrib(image.GetSamplingDistribution());
//	RNG rng;
//	for (int i = 0; i < 1000; ++i) {
//		Point2f u(rng.Uniform<float>(), rng.Uniform<float>());
//		float pdf;
//		Point2f p = distrib.Sample(u, &pdf);
//		float fp = f(p);
//		// Allow some error since PiecewiseConstant2D uses a piecewise constant
//		// sampling distribution.
//		EXPECT_LE(std::abs(fp - pdf * integral), .02f)
//			<< u << ", fp: " << fp << ", pdf: " << pdf;
//	}
//}

TEST(Image, Wrap2D) {
	std::vector<float> texels = { float(0), float(1), float(0), float(0), float(0),
		float(0), float(0), float(0), float(0) };
	Image zeroOne(texels, { 3, 3 }, std::vector<std::string>{ "Y" });

	EXPECT_EQ(1, zeroOne.GetChannel({ 1, -1 }, 0, { WrapMode::Clamp, WrapMode::Clamp }));
	EXPECT_EQ(1, zeroOne.GetChannel({ 1, -1 }, 0, { WrapMode::Black, WrapMode::Clamp }));
	EXPECT_EQ(0, zeroOne.GetChannel({ 1, -1 }, 0, { WrapMode::Black, WrapMode::Repeat }));
	EXPECT_EQ(0, zeroOne.GetChannel({ 1, -1 }, 0, { WrapMode::Clamp, WrapMode::Black }));

	EXPECT_EQ(0, zeroOne.GetChannel({ 1, 3 }, 0, { WrapMode::Clamp, WrapMode::Clamp }));
	EXPECT_EQ(0, zeroOne.GetChannel({ 1, 3 }, 0, { WrapMode::Repeat, WrapMode::Clamp }));
	EXPECT_EQ(1, zeroOne.GetChannel({ 1, 3 }, 0, { WrapMode::Black, WrapMode::Repeat }));
	EXPECT_EQ(0, zeroOne.GetChannel({ 1, 3 }, 0, { WrapMode::Clamp, WrapMode::Black }));

	EXPECT_EQ(0.5, zeroOne.BilerpChannel(Vector2f(0.5, 0.), 0, WrapMode::Repeat));
	EXPECT_EQ(0.5, zeroOne.BilerpChannel(Vector2f(0.5, 0.), 0, WrapMode::Black));
	EXPECT_EQ(1, zeroOne.BilerpChannel(Vector2f(0.5, 0.), 0, WrapMode::Clamp));
}

TEST(Image, Select) {
	Vector2i res(4, 9);
	std::vector<float> pix = GetFloatPixels(res, 4);
	Image image(pix, res, std::vector<std::string>{ "A", "B", "G", "R" });

	auto aDesc = image.GetChannelDesc({ "A" });
	EXPECT_TRUE(bool(aDesc));
	Image aImage = image.SelectChannels(aDesc);

	EXPECT_EQ(aImage.Resolution, image.Resolution);
	EXPECT_EQ(1, aImage.NChannels());
	EXPECT_EQ(1, aImage.ChannelNames.size());
	EXPECT_EQ("A", aImage.ChannelNames[0]);
	for (int y = 0; y < res.Y; ++y)
		for (int x = 0; x < res.X; ++x)
			EXPECT_EQ(aImage.GetChannel({ x, y }, 0), image.GetChannel({ x, y }, 0));

	auto rgDesc = image.GetChannelDesc({ "R", "G" });
	EXPECT_TRUE(bool(rgDesc));
	Image rgImage = image.SelectChannels(rgDesc);
	EXPECT_EQ(rgImage.Resolution, image.Resolution);
	EXPECT_EQ(2, rgImage.NChannels());
	EXPECT_EQ(2, rgImage.ChannelNames.size());
	EXPECT_EQ("R", rgImage.ChannelNames[0]);
	EXPECT_EQ("G", rgImage.ChannelNames[1]);
	for (int y = 0; y < res.Y; ++y)
		for (int x = 0; x < res.X; ++x) {
			EXPECT_EQ(rgImage.GetChannel({ x, y }, 0), image.GetChannel({ x, y }, 3));
			EXPECT_EQ(rgImage.GetChannel({ x, y }, 1), image.GetChannel({ x, y }, 2));
		}
}
//
/////////////////////////////////////////////////////////////////////////////
//
static std::string inTestDir(const std::string &path) {
	return path;
}

static void TestRoundTrip(const char *fn) {
	Vector2i res(16, 29);
	Image image(PixelFormat::Float, res, std::vector<std::string>{ "R", "G", "B" });
	for (int y = 0; y < res[1]; ++y)
		for (int x = 0; x < res[0]; ++x)
			image.SetChannels({ x, y }, std::vector<float>{ float(x) / float(res[0] - 1),
				float(y) / float(res[1] - 1), float(-1.5) });

	std::string filename = inTestDir(fn);
	ASSERT_TRUE(image.Write(filename));

	ImageAndMetadata readImage = Image::Read(filename);
	ASSERT_EQ(readImage.image.Resolution, res);

	ImageChannelDesc rgbDesc = readImage.image.GetChannelDesc({ "R", "G", "B" });
	ASSERT_TRUE((bool)rgbDesc);

	for (int y = 0; y < res[1]; ++y)
		for (int x = 0; x < res[0]; ++x) {
			ImageChannelValues rgb = readImage.image.GetChannels({ x, y }, rgbDesc);

			for (int c = 0; c < 3; ++c) {
				float wrote = image.GetChannel({ x, y }, c);
				float delta = wrote - rgb[c];
				//if (HasExtension(filename, "pfm")) 
				if(false)
				{
					// Everything should come out exact.
					EXPECT_EQ(0, delta) << filename << ":(" << x << ", " << y
						<< ") c = " << c << " wrote " << wrote
						<< ", read " << rgb[c] << ", delta = " << delta;
				}
				//else if (HasExtension(filename, "exr")) 
				else if (false)
				{
					if (c == 2)
						// -1.5 is exactly representable as a float.
						EXPECT_EQ(0, delta)
						<< "(" << x << ", " << y << ") c = " << c << " wrote "
						<< wrote << ", read " << rgb[c] << ", delta = " << delta;
					else
						EXPECT_LT(std::abs(delta), .001)
						<< filename << ":(" << x << ", " << y << ") c = " << c
						<< " wrote " << wrote << ", read " << rgb[c]
						<< ", delta = " << delta;
				}
				else 
				{
					// 8 bit format...
					if (c == 2)
						// -1.5 should be clamped to zero.
						EXPECT_EQ(0, rgb[c])
						<< "(" << x << ", " << y << ") c = " << c << " wrote "
						<< wrote << ", read " << rgb[c] << " (expected 0 back)";
					else
						// Allow a fair amount of slop, since there's an sRGB
						// conversion before quantization to 8-bits...
						EXPECT_LT(std::abs(delta), .02)
						<< filename << ":(" << x << ", " << y << ") c = " << c
						<< " wrote " << wrote << ", read " << rgb[c]
						<< ", delta = " << delta;
				}
			}
		}

	// Clean up
	EXPECT_EQ(0, remove(filename.c_str()));
}

//TEST(ImageIO, RoundTripEXR) {
//	TestRoundTrip("out.exr");
//}
//
//TEST(ImageIO, RoundTripPFM) {
//	TestRoundTrip("out.pfm");
//}

TEST(ImageIO, RoundTripPNG) {
	TestRoundTrip("out.png");
}