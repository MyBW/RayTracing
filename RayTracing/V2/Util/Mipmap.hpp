#pragma once
#include <string>
#include "Image.hpp"
#include <memory>

namespace BlackWalnut
{
	

	enum class FilterFunction { Point, Bilinear, Trilinear, EWA };

	inline FilterFunction ParseFilter(const std::string& f) {
		if (f == "ewa" || f == "EWA")
			return FilterFunction::EWA;
		else if (f == "trilinear")
			return FilterFunction::Trilinear;
		else if (f == "bilinear")
			return FilterFunction::Bilinear;
		else if (f == "point")
			return FilterFunction::Point;
		else
			CHECK(0);
	}
	struct MIPMapFilterOptions
	{
		FilterFunction Filter = FilterFunction::EWA;
		float MaxAnisotropy = 8.f;
	};
	class MIPMap
	{
	public:
		MIPMap(Image InImage, const RGBColorSpace* ColorSpace, WrapMode InWrapMode, const MIPMapFilterOptions& Option);
		static std::unique_ptr<MIPMap> CreateFromFile(const std::string& FileName, const MIPMapFilterOptions& Option, WrapMode InWrapMode,
			const ColorEncodingBase* ColorEncoding);
		template<typename T>
		T Lookup(const Vector2f& st, float Width = 0.f) const
		{
			// Compute MIPMap level
			int nLevels = Levels();
			float level = nLevels - 1 + Log2(std::max<float>(Width, 1e-8));

			if (level >= Levels() - 1)
				return Texel<T>(Levels() - 1, { 0, 0 });

			int iLevel = std::max(0, int(std::floor(level)));
			if (Options.Filter == FilterFunction::Point) {
				Vector2i resolution = LevelResolution(iLevel);
				Vector2i sti(std::round(st[0] * resolution[0] - 0.5f),
					std::round(st[1] * resolution[1] - 0.5f));
				return Texel<T>(iLevel, sti);
			}
			else if (Options.Filter == FilterFunction::Bilinear) {
				return Bilerp<T>(iLevel, st);
			}
			else {
				CHECK(Options.Filter == FilterFunction::Trilinear);

				if (iLevel == 0)
					return Bilerp<T>(0, st);
				else {
					float delta = level - iLevel;
					CHECK_LE(delta, 1);
					return Lerp(Bilerp<T>(iLevel, st), Bilerp<T>(iLevel + 1, st), delta);
				}
			}
		}
		template <typename T>
		T Lookup(const Vector2f& st, Vector2f dst0, Vector2f dst1) const
		{
			if (Options.Filter != FilterFunction::EWA) {
				float width = std::max(
					{ std::abs(dst0[0]), std::abs(dst0[1]), std::abs(dst1[0]), std::abs(dst1[1]) });
				return Lookup<T>(st, 2 * width);
			}
			// Compute ellipse minor and major axes
			if (Length(dst0) * Length(dst0) < Length(dst1) * Length(dst1))
				std::swap(dst0, dst1);
			float majorLength = Length(dst0);
			float minorLength = Length(dst1);

			// Clamp ellipse eccentricity if too large
			if (minorLength * Options.MaxAnisotropy < majorLength && minorLength > 0) {
				float scale = majorLength / (minorLength * Options.MaxAnisotropy);
				dst1 = dst1 * scale;
				minorLength *= scale;
			}
			if (minorLength == 0)
				return Bilerp<T>(0, st);

			// Choose level of detail for EWA lookup and perform EWA filtering
			float lod = std::max<float>(0, Levels() - 1 + Log2(minorLength));
			int ilod = std::floor(lod);
			// TODO: just do return EWA<T>(ilog, st, dst0, dst1);
			// TODO: also, when scaling camera ray differentials, just do e.g.
			// 1 / std::min(sqrtSamplesPerPixel, 8);
			return (EWA<T>(ilod, st, dst0, dst1) * (1 - (lod - ilod)) +
				EWA<T>(ilod + 1, st, dst0, dst1) * (lod - ilod));
		}
		uint32_t Levels() const { return uint32_t(Pyramid.size()); }

		Vector2i LevelResolution(int level) const {
			CHECK(level >= 0 && level < Pyramid.size());
			return Pyramid[level].Resolution;
		}
		const RGBColorSpace* GetRGBColorSpace() const { return ColorSpace; }
		Image& GetLevelImage(int Level)
		{
			CHECK(Level >= 0 && Level < Pyramid.size());
			return Pyramid[Level];
		}
	private:
		template <typename T>
		T Texel(int level, Vector2i st) const;
		template <typename T>
		T Bilerp(int level, Vector2f st) const;
		template <typename T>
		T EWA(int level, Vector2f st, Vector2f dst0, Vector2f dst1) const;

		std::vector<Image> Pyramid;
		const RGBColorSpace* ColorSpace;
		WrapMode mWrapMode;
		MIPMapFilterOptions Options;

		static const int WeightLUTSize = 128;
		static const float weightLut[WeightLUTSize];
	};




	
	
	template <typename T>
	T MIPMap::EWA(int level, Vector2f st, Vector2f dst0, Vector2f dst1) const {
		if (level >= Levels())
			return Texel<T>(Levels() - 1, { 0, 0 });

		// Convert EWA coordinates to appropriate scale for level
		Vector2i levelRes = LevelResolution(level);
		st[0] = st[0] * levelRes[0] - 0.5f;
		st[1] = st[1] * levelRes[1] - 0.5f;
		dst0[0] *= levelRes[0];
		dst0[1] *= levelRes[1];
		dst1[0] *= levelRes[0];
		dst1[1] *= levelRes[1];

		// Compute ellipse coefficients to bound EWA filter region
		float A = dst0[1] * dst0[1] + dst1[1] * dst1[1] + 1;
		float B = -2 * (dst0[0] * dst0[1] + dst1[0] * dst1[1]);
		float C = dst0[0] * dst0[0] + dst1[0] * dst1[0] + 1;
		float invF = 1 / (A * C - B * B * 0.25f);
		A *= invF;
		B *= invF;
		C *= invF;

		// Compute the ellipse's $(s,t)$ bounding box in texture space
		float det = -B * B + 4 * A * C;
		float invDet = 1 / det;
		float uSqrt = Sqrt(det * C), vSqrt = Sqrt(A * det);
		int s0 = std::ceil(st[0] - 2 * invDet * uSqrt);
		int s1 = std::floor(st[0] + 2 * invDet * uSqrt);
		int t0 = std::ceil(st[1] - 2 * invDet * vSqrt);
		int t1 = std::floor(st[1] + 2 * invDet * vSqrt);

		// Scan over ellipse bound and compute quadratic equation
		T sum{};
		float sumWts = 0;
		for (int it = t0; it <= t1; ++it) {
			float tt = it - st[1];
			for (int is = s0; is <= s1; ++is) {
				float ss = is - st[0];
				// Compute squared radius and filter texel if inside ellipse
				float r2 = A * ss * ss + B * ss * tt + C * tt * tt;
				if (r2 < 1) {
					int index = std::min<int>(r2 * WeightLUTSize, WeightLUTSize - 1);
					float weight = weightLut[index];
					sum +=  Texel<T>(level, Vector2i(is, it)) * weight;
					sumWts += weight;
				}
			}
		}
		return sum / sumWts;
	}


}