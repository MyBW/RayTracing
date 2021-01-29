#pragma once 

#include "..\Math\Math.hpp"
#include <cmath>
#include <algorithm>
#include "Rng.hpp"
namespace BlackWalnut
{
	template<typename Iterator>
	class IndexingIterator
	{
	public:
		template<typename Generator>
		IndexingIterator(int Ini, int Inn, const Generator* G):i(Ini),n(Inn) { }
		bool operator==(const Iterator& It) const
		{
			return It.i == i;
		}
		bool operator!=(const Iterator& It) const
		{
			return It.i != i;
		}
		Iterator& operator++()
		{
			i++;
			return (Iterator &)*this;
		}
		Iterator operator++(int) const
		{
			Iterator It = *this;
			++It;
			return It;
		}

	protected:
		int32_t i, n;
	};
	template<typename Generator, typename Iterator>
	class IndexingGenerator
	{
	public:
		IndexingGenerator(int32_t n):n(n) { }
		Iterator begin() const
		{
			return Iterator(0, n, (const Generator*) this);
		}
		Iterator end() const
		{
			return Iterator(n, n, (const Generator*) this);
		}
	protected:
		int32_t n;
	};

	template <typename Iterator>
	class RNGIterator;

	template<typename Generator, typename Iterator>
	class RNGGenerator : public IndexingGenerator<Generator, Iterator>
	{
	public:
		RNGGenerator(int32_t n, uint64_t SequenceIndx = 0, uint64_t Seed = PCG32_DEFAULT_STATE)
			:IndexingGenerator<Generator, Iterator>(n),SequenceIndx(SequenceIndx),Seed(Seed) { }
	protected:
		friend RNGIterator<Iterator>;
		uint64_t Seed;
		uint64_t SequenceIndx;
	};
	template<typename Iterator>
	class RNGIterator : public IndexingIterator<Iterator>
	{
	public:
		template<typename Generator>
		RNGIterator(int32_t i, int32_t n, const RNGGenerator<Generator,Iterator> *G) 
			:IndexingIterator<Iterator>(i, n, G),Rng(G->SequenceIndx)
		{

		}
	protected:
		RNG Rng;
	};

	class Stratified1DItor : public RNGIterator<Stratified1DItor>
	{
	public:
		using RNGIterator<Stratified1DItor>::RNGIterator;
		float operator*() { return (i + Rng.Uniform<float>()) / n; }
	};
	class Stratified1D : public RNGGenerator<Stratified1D, Stratified1DItor>
	{
	public:
		using RNGGenerator<Stratified1D, Stratified1DItor>::RNGGenerator;
	};
	class PiecewiseConstant2D
	{
	public:

	};
	inline float XYZMatchingPDF(float lambda) 
	{
		if (lambda < 360 || lambda > 830)
			return 0;
		return 0.0039398042f / Square(std::cosh(0.0072f * (lambda - 538)));
	}
	inline float SampleXYZMatching(float u) 
	{
		return 538 - 138.888889f * std::atanh(0.85691062f - 1.82750197f * u);
	}

	
	inline Vector3f SampleUniformSphere(const Vector2f &u) {
		float z = 1 - 2 * u[0];
		float r = SafeSqrt(1 - z * z);
		float phi = 2 * Pi * u[1];
		return{ r * std::cos(phi), r * std::sin(phi), z };
	}

	
	inline float UniformSpherePDF() 
	{
		return Inv4Pi;
	}

	inline Vector3f SampleUniformHemisphere(const Vector2f &u) 
	{
		float z = u[0];
		float r = SafeSqrt(1 - z * z);
		float phi = 2 * Pi * u[1];
		return{ r * std::cos(phi), r * std::sin(phi), z };
	}
	inline float UniformHemispherePDF() 
	{
		return Inv2Pi;
	}

	inline int SampleDiscrete(std::vector<float> weights, float u, float *pdf = nullptr, float *uRemapped = nullptr) 
	{
		// Handle empty _weights_ for discrete sampling
		if (weights.empty()) {
			if (pdf != nullptr)
				*pdf = 0;
			return -1;
		}

		// Compute sum of _weights_
		float sumWeights = 0;
		for (float w : weights)
			sumWeights += w;

		// Find offset in _weights_ corresponding to _u_
		int offset = 0;
		while (offset < weights.size() && u >= weights[offset] / sumWeights) {
			u -= weights[offset] / sumWeights;
			++offset;
		}
		//CHECK_RARE(1e-6, offset == weights.size());
		if (offset == weights.size())
			offset = weights.size() - 1;

		// Compute PDF and remapped _u_ value, if necessary
		float p = weights[offset] / sumWeights;
		if (pdf != nullptr)
			*pdf = p;
		if (uRemapped != nullptr)
			*uRemapped = (std::min)(u / p, OneMinusEpsilon);

		return offset;
	}
	inline Vector2f SampleUniformDiskConcentric(const Vector2f &u) {
		// Map _u_ to $[-1,1]^2$ and handle degeneracy at the origin
		Vector2f uOffset =  u * 2.f - Vector2f(1, 1);
		if (uOffset.X == 0 && uOffset.Y == 0)
			return{ 0, 0 };

		// Apply concentric mapping to point
		float theta, r;
		if (std::abs(uOffset.X) > std::abs(uOffset.Y)) {
			r = uOffset.X;
			theta = PiOver4 * (uOffset.Y / uOffset.X);
		}
		else {
			r = uOffset.Y;
			theta = PiOver2 - PiOver4 * (uOffset.X / uOffset.Y);
		}
		return Vector2f(std::cos(theta), std::sin(theta)) * r;
	}
	inline Vector3f SampleCosineHemisphere(const Vector2f &u) {
		Vector2f d = SampleUniformDiskConcentric(u);
		float z = SafeSqrt(1 - d.X * d.X - d.Y * d.Y);
		return Vector3f(d.X, d.Y, z);
	}
	inline Vector3f SphericalDirection(float sinTheta, float cosTheta, float phi) {
		CHECK(sinTheta >= -1.0001 && sinTheta <= 1.0001);
		CHECK(cosTheta >= -1.0001 && cosTheta <= 1.0001);
		return Vector3f(Clamp(sinTheta, -1, 1) * std::cos(phi),
			Clamp(sinTheta, -1, 1) * std::sin(phi), Clamp(cosTheta, -1, 1));
	}
	inline Vector3f SampleTrowbridgeReitzVisibleArea(const Vector3f &w, float alpha_x, float alpha_y, const Vector2f &u) 
	{
		// Section 3.2: transforming the view direction to the hemisphere
		// configuration
		Vector3f wh = Vector3f(alpha_x * w.X, alpha_y * w.Y, w.Z);
		Normalize(wh);

		// Section 4.1: orthonormal basis. Can't use CoordinateSystem() since
		// T1 has to be in the tangent plane w.r.t. (0,0,1).
		Vector3f temp;
		CrossProduct(temp, Vector3f(0.0f, 0.0f, 1.f), wh);

		Vector3f T1 =
			(wh.Z < 0.99999f) ? temp : Vector3f(1, 0, 0);
		Vector3f T2;
		CrossProduct(T2,wh, T1);

		// Section 4.2: parameterization of the projected area
		float r = std::sqrt(u[0]);
		float phi = 2 * Pi * u[1];
		float t1 = r * std::cos(phi), t2 = r * std::sin(phi);
		float s = 0.5f * (1 + wh.Z);
		t2 = (1 - s) * std::sqrt(1 - t1 * t1) + s * t2;

		// Section 4.3: reprojection onto hemisphere
		Vector3f nh =
			T1 * t1 +  T2 * t2 + wh * std::sqrt(std::max<float>(0, 1 - t1 * t1 - t2 * t2));

		// Section 3.4: transforming the normal back to the ellipsoid configuration
		//CHECK_RARE(1e-6, nh.z == 0);
		temp = Vector3f(alpha_x * nh.X, alpha_y * nh.Y, std::max<float>(1e-6f, nh.Z));
		Normalize(temp);
		return temp;
	}

	inline Vector3f SampleTrowbridgeReitz(float alpha_x, float alpha_y, const Vector2f &u) {
		float cosTheta = 0, phi = (2 * Pi) * u[1];
		if (alpha_x == alpha_y) {
			float tanTheta2 = alpha_x * alpha_x * u[0] / (1 - u[0]);
			cosTheta = 1 / std::sqrt(1 + tanTheta2);
		}
		else {
			phi = std::atan(alpha_y / alpha_x * std::tan(2 * Pi * u[1] + .5f * Pi));
			if (u[1] > .5f)
				phi += Pi;
			float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
			float alpha2 = 1 / (Pow(cosPhi / alpha_x, 2) + Pow(sinPhi / alpha_y, 2));
			float tanTheta2 = alpha2 * u[0] / (1 - u[0]);
			cosTheta = 1 / std::sqrt(1 + tanTheta2);
		}
		float sinTheta = SafeSqrt(1 - cosTheta * cosTheta);
		return SphericalDirection(sinTheta, cosTheta, phi);
	}
}
