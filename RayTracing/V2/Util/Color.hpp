#pragma once
#include "..\V2\Math\Geom.hpp"
#include "..\V2\Math\Matrix.hpp"
#include "..\V2\Math\MatrixMath.hpp"
#include "..\Math\Math.hpp"
#include "Check.h"
#include <array>
#include <algorithm>
#include <vector>

namespace BlackWalnut
{
	struct PiecewiseLinearSegment {
		float base, slope;
	};
	constexpr int LinearToSRGBPiecewiseSize = 256;
	extern const PiecewiseLinearSegment LinearToSRGBPiecewise[];
	extern const float SRGBToLinearLUT[256];

	// White Balance Definitions
	// clang-format off
	// These are the Bradford transformation matrices.
	const Matrix3X3f LMSFromXYZ = { 0.8951, 0.2664, -0.1614,
		-0.7502, 1.7135, 0.0367,
		0.0389, -0.0685, 1.0296 };
	const Matrix3X3f XYZFromLMS = { 0.986993, -0.147054, 0.159963,
		0.432305, 0.51836, 0.0492912,
		-0.00852866, 0.0400428, 0.968487 };
	

	inline float LinearToSRGB(float value) {
		int index = int(value * LinearToSRGBPiecewiseSize);
		if (index < 0)
			return 0;
		if (index >= LinearToSRGBPiecewiseSize)
			return 1;
		return LinearToSRGBPiecewise[index].base + value * LinearToSRGBPiecewise[index].slope;
	}


	inline float LinearToSRGBFull(float value) {
		if (value <= 0.0031308f)
			return 12.92f * value;
		return 1.055f * std::pow(value, (float)(1.f / 2.4f)) - 0.055f;
	}

	inline float SRGBToLinear(float value) {
		if (value <= 0.04045f)
			return value * (1 / 12.92f);
		return std::pow((value + 0.055f) * (1 / 1.055f), (float)2.4f);
	}

	
	inline float SRGB8ToLinear(uint8_t value)
	{
		return SRGBToLinearLUT[value];
	}
	inline uint8_t LinearToSRGB8(float value, float dither = 0) {
		if (value <= 0)
			return 0;
		if (value >= 1)
			return 255;
		return Clamp(255.f * LinearToSRGB(value) + dither, 0.0f, 255.0f);
	}

	class ColorEncodingBase
	{
	public:
		virtual void ToLinear(std::vector<uint8_t> vin, std::vector<float>& vout) const = 0;

		virtual inline float ToFloatLinear(float v) const =  0;

		virtual void FromLinear(std::vector<float> vin, std::vector<uint8_t>& vout) const = 0;


		static const ColorEncodingBase* Linear;
		static const ColorEncodingBase* sRGB;

		static const ColorEncodingBase* Get(const std::wstring &name, float gamma = 1.0);
	};
	class LinearColorEncoding : public ColorEncodingBase
	{
	public:
		void ToLinear(std::vector<uint8_t> vin, std::vector<float>& vout) const override
		{
			for (size_t i = 0; i < vin.size(); ++i)
				vout[i] = vin[i] / 255.f;
		}
		void FromLinear(std::vector<float> vin, std::vector<uint8_t>& vout) const override
		{
			for (size_t i = 0; i < vin.size(); ++i)
				vout[i] = uint8_t(Clamp(vin[i] * 255.f + 0.5f, 0.f, 255.f));
		}
		float ToFloatLinear(float v) const override { return v; }
	};
	class sRGBColorEncoding : public ColorEncodingBase
	{
	public:
		void ToLinear(std::vector<uint8_t> vin, std::vector<float>& vout) const override;
		void FromLinear(std::vector<float> vin, std::vector<uint8_t>& vout) const override;
		float ToFloatLinear(float v) const override;
	};
	class GammaColorEncodeing : public ColorEncodingBase
	{
	public:
		GammaColorEncodeing(float gamma);
		void ToLinear(std::vector<uint8_t> vin, std::vector<float>& vout) const override;
		void FromLinear(std::vector<float> vin, std::vector<uint8_t>& vout) const override;
		float ToFloatLinear(float v) const override;
	private:
		float gamma;
		std::array<float, 256> applyLUT;
		std::array<float, 1024> inverseLUT;
	};
#undef RGB

	class RGB
	{
	public:
		RGB() = default;
		RGB(float InX, float InY, float InZ)
			:X(InX), Y(InY), Z(InZ)
		{

		}
		RGB& operator+=(const RGB &S)
		{
			X += S.X;
			Y += S.Y;
			Z += S.Z;
			return *this;
		}
		RGB& operator-=(const RGB &S)
		{
			X -= S.X;
			Y -= S.Y;
			Z -= S.Z;
			return *this;
		}
		RGB& operator*=(const RGB &S)
		{
			X *= S.X;
			Y *= S.Y;
			Z *= S.Z;
			return *this;
		}
		RGB& operator/=(const RGB &S)
		{
			X /= S.X;
			Y /= S.Y;
			Z /= S.Z;
			return *this;
		}
		RGB& operator/=(const float S)
		{
			X /= S;
			Y /= S;
			Z /= S;
			return *this;
		}
		RGB& operator*=(const float S)
		{
			X *= S;
			Y *= S;
			Z *= S;
			return *this;
		}
		RGB operator+(const RGB &S) const
		{
			RGB Ret = *this;
			return Ret += S;
		}
		RGB operator-(const RGB &S) const
		{
			RGB Ret = *this;
			return Ret -= S;
		}
		RGB operator*(const RGB &S) const
		{
			RGB Ret = *this;
			return Ret *= S;
		}
		RGB operator/(const RGB &S) const
		{
			RGB Ret = *this;
			return Ret /= S;
		}
		RGB operator*(const float &S)
		{
			return{ X * S, Y * S, Z * S };
		}
		RGB operator/(const float &S)  const
		{
			return{ X / S, Y / S, Z / S };
		}
		float& operator[](int I)
		{
			if (I == 0) return X;
			if (I == 1) return Y;
			if (I == 2) return Z;
		}
		float operator[](int I) const
		{
			if (I == 0) return X;
			if (I == 1) return Y;
			if (I == 2) return Z;
		}
		float Average() const
		{
			return (X + Y + Z) / 3.0f;
		}
		float X, Y, Z;
	};


	class XYZ
	{
	public:
		XYZ() = default;
		XYZ(float InX, float InY, float InZ)
			:X(InX), Y(InY), Z(InZ)
		{

		}
		XYZ& operator+=(const XYZ &S)
		{
			X += S.X;
			Y += S.Y;
			Z += S.Z;
			return *this;
		}
		XYZ& operator-=(const XYZ &S)
		{
			X -= S.X;
			Y -= S.Y;
			Z -= S.Z;
			return *this;
		}
		XYZ& operator*=(const XYZ &S)
		{
			X *= S.X;
			Y *= S.Y;
			Z *= S.Z;
			return *this;
		}
		XYZ& operator/=(const XYZ &S)
		{
			X /= S.X;
			Y /= S.Y;
			Z /= S.Z;
			return *this;
		}
		XYZ& operator*=(const float S)
		{
			X *= S;
			Y *= S;
			Z *= S;
			return *this;
		}
		XYZ operator+(const XYZ &S) const 
		{
			XYZ Ret = *this;
			return Ret += S;
		}
		XYZ operator-(const XYZ &S) const
		{
			XYZ Ret = *this;
			return Ret -= S;
		}
		XYZ operator*(const XYZ &S) const
		{
			XYZ Ret = *this;
			return Ret *= S;
		}
		XYZ operator/(const XYZ &S) const
		{
			XYZ Ret = *this;
			return Ret /= S;
		}
		XYZ operator/(const float S) const
		{
			CHECK(S);
			XYZ Ret = *this;
			Ret.X /= S;
			Ret.Y /= S;
			Ret.Z /= S;
			return Ret;
		}
		XYZ operator*(const float &S)
		{
			X *= S;
			Y *= S;
			Z *= S;
			return { X * S, Y * S, Z * S };
		}
		float& operator[](int I)
		{
			if (I == 0) return X;
			if (I == 1) return Y;
			if (I == 2) return Z;
		}
		float operator[](int I) const
		{
			if (I == 0) return X;
			if (I == 1) return Y;
			if (I == 2) return Z;
		}
		float Average() const
		{
			return (X + Y + Z) / 3.0f;
		}
		Vector2f xy()
		{
			return Vector2f(X / (X + Y + Z), Y / (X + Y + Z));
		}
		static XYZ FromxyY(Vector2f XY, float Y = 1)
		{
			if (XY.Y == 0)
				return XYZ(0.0f, 0.0f, 0.0f);
			return XYZ(XY.X * Y / XY.Y, Y, (1 - XY.X - XY.Y) * Y / XY.Y);
		}
		float X, Y, Z;
	};

	class RGBSigmoidPolynomial
	{
	public:
		RGBSigmoidPolynomial() = default;
		RGBSigmoidPolynomial(float InC0, float InC1, float InC2):C0(InC0),C1(InC1),C2(InC2) {}
		float operator()(float Lambd) const
		{
			float v = EvaluatePolynomial(Lambd, C2, C1, C0);
			if (IsInf(v))
				return v > 0 ? 1 : 0;
			return s(v);
		}
		float MaxValue() const
		{
			if (C0 < 0)
			{
				float lambda = -C1 / (2 * C0);
				if (lambda >= 360 && lambda <= 830)
					return (std::max)({ (*this)(lambda), (*this)(360), (*this)(830) });
					
			}
			return (std::max)((*this)(360), (*this)(830));
		}
	private:
		
		static float s(float x) { return .5f + x / (2 * std::sqrt(1 + x * x)); };

		float C0;
		float C1;
		float C2;
	};

	class RGBToSpectrumTable
	{
	public:
		RGBToSpectrumTable(int InRes, const float *InScale, const float *InData)
			:Res(InRes),Scale(InScale),Data(InData)
		{

		}
		static void Init();
		RGBSigmoidPolynomial operator()(const RGB& Rgb) const 
		{
			CHECK(Rgb[0] >= 0.f && Rgb[1] >= 0.f && Rgb[2] >= 0.f && Rgb[0] <= 1.f &&
				Rgb[1] <= 1.f && Rgb[2] <= 1.f);

			// Find largest RGB component and handle black _rgb_
			int i = 0;
			for (int j = 1; j < 3; ++j)
				if (Rgb[j] >= Rgb[i])
					i = j;
			if (Rgb[i] == 0)
				return{ float(0), float(0), -std::numeric_limits<float>::infinity() };

			// Compute floating-point offsets into polynomial coefficient table
			float z = Rgb[i], sc = (Res - 1) / z, x = Rgb[(i + 1) % 3] * sc,
				y = Rgb[(i + 2) % 3] * sc;

			// Compute integer indices and offsets for coefficient interpolation
			constexpr int nCoeffs = 3;
			int xi = (std::min)((int)x, Res - 2), yi = (std::min)((int)y, Res - 2),
				zi = FindInterval(Res, [&](int i) { return Scale[i] < z; }),
				offset = (((i * Res + zi) * Res + yi) * Res + xi) * nCoeffs, dx = nCoeffs,
				dy = nCoeffs * Res, dz = nCoeffs * Res * Res;
			float x1 = x - xi, x0 = 1 - x1, y1 = y - yi, y0 = 1 - y1,
				z1 = (z - Scale[zi]) / (Scale[zi + 1] - Scale[zi]), z0 = 1 - z1;

			// Bilinearly interpolate sigmoid polynomial coefficients _c_
			std::array<float, nCoeffs> c;

			for (int j = 0; j < nCoeffs; ++j) {
				c[j] = ((Data[offset] * x0 + Data[offset + dx] * x1) * y0 +
					(Data[offset + dy] * x0 + Data[offset + dy + dx] * x1) * y1) *
					z0 +
					((Data[offset + dz] * x0 + Data[offset + dz + dx] * x1) * y0 +
					(Data[offset + dz + dy] * x0 + Data[offset + dz + dy + dx] * x1) * y1) *
					z1;
				offset++;
			}

			return RGBSigmoidPolynomial(c[0], c[1], c[2]);
		}
		static const RGBToSpectrumTable *sRGB;
		static const RGBToSpectrumTable *DCI_P3;
		static const RGBToSpectrumTable *Rec2020;
		static const RGBToSpectrumTable *ACES2065_1;
	private:
		int Res;
		const float *Scale;
		const float *Data;
	};
	inline Matrix3X3f WhiteBalance(Vector2f SrcWhite, Vector2f DesWhite)
	{
		XYZ SrcXYZ = XYZ::FromxyY(SrcWhite);
		XYZ DesXYZ = XYZ::FromxyY(DesWhite);
		auto SrcLMS = Mul<XYZ, 3, XYZ>(LMSFromXYZ, SrcXYZ);
		auto DesLMS = Mul<XYZ, 3, XYZ>(LMSFromXYZ, DesXYZ);
		Matrix3X3f LMSCorrect;
		LMSCorrect.MakeZero();
		LMSCorrect[0][0] = DesLMS[0] / SrcLMS[0];
		LMSCorrect[1][1] = DesLMS[1] / SrcLMS[1];
		LMSCorrect[2][2] = DesLMS[2] / SrcLMS[2];
		return XYZFromLMS * LMSCorrect * LMSFromXYZ;
	}

	inline RGB ClampZero(const RGB &Rgb)
	{
		return RGB(std::max<float>(0, Rgb.X), std::max<float>(0, Rgb.Y), std::max<float>(0, Rgb.Z));
	}

}