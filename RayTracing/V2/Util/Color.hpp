#pragma once
#include "..\V2\Math\Geom.hpp"
#include "..\V2\Math\Matrix.hpp"
#include "..\V2\Math\MatrixMath.hpp"
#include "Check.h"

namespace BlackWalnut
{
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
		RGB operator/(const float &S)
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
	class RGBToSpectrumTable
	{
	public:
		RGBToSpectrumTable(int InRes, const float *InScale, const float *InData)
			:Res(InRes),Scale(InScale),Data(InData)
		{

		}
		static void Init();
		static const RGBToSpectrumTable *sRGB;
		static const RGBToSpectrumTable *DCI_P3;
		static const RGBToSpectrumTable *Rec2020;
		static const RGBToSpectrumTable *ACES2065_1;
	private:
		int Res;
		const float *Scale;
		const float *Data;
	};
	// White Balance Definitions
	// clang-format off
	// These are the Bradford transformation matrices.
	const Matrix3X3f LMSFromXYZ = { 0.8951, 0.2664, -0.1614,
		-0.7502, 1.7135, 0.0367,
		0.0389, -0.0685, 1.0296 };
	const Matrix3X3f XYZFromLMS = { 0.986993, -0.147054, 0.159963,
		0.432305, 0.51836, 0.0492912,
		-0.00852866, 0.0400428, 0.968487 };
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
}