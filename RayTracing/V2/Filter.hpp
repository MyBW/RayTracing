#pragma once
#include "Math\Geom.hpp"
#include "BWPrimitive.h"
#include "Util\Sampling.hpp"
#include "Math\Math.hpp"

namespace BlackWalnut
{

	// FilterSample Definition
	struct FilterSample {
		Vector2f P;
		float Weight;
	};
	class FilterBase
	{
	public:
		virtual float Integral() const = 0;
		virtual Vector2f Radius() const = 0;
		virtual float Evaluate(const Vector2f &p) const = 0;
		virtual FilterSample Sample(const Vector2f &u) const = 0;
	};
	

	//class FilterSampler 
	//{
	//public:
	//	// FilterSampler Public Methods
	//	FilterSampler(){}

	//	template<typename FilterType>
	//	FilterSampler(FilterType Filter);
	//	FilterSample Sample(const Vector2f &u) const 
	//	{
	//		Vector2f p = Distrib.Sample(u);
	//		Vector2f p01 = Vector2f(Domain.Offset(p));
	//		Vector2i pi(Clamp(p01.X * Values.XSize() + 0.5f, 0, Values.XSize() - 1),
	//			Clamp(p01.Y * Values.YSize() + 0.5f, 0, Values.YSize() - 1));
	//		return{ p, Values[pi] < 0 ? -1.f : 1.f };
	//	}
	//	float Integral() const { return Distrib.Integral(); }

	//private:
	//	// FilterSampler Private Members
	//	Bounds2f Domain;
	//	Array2D<float> Values;
	//	PiecewiseConstant2D Distrib;
	//};


	//class GaussianFilter
	//{
	//public:
	//	GaussianFilter(const Vector2f &Radius, float Sigma = 0.5f):Radius(Radius),Sigma(Sigma),
	//		ExpX(Gaussian(Radius.X, 0, Sigma)),
	//		ExpY(Gaussian(Radius.Y, 0, Sigma))
	//	{
	//			Sampler = FilterSampler<GaussianFilter>();
	//	}

	//private:
	//	// GaussianFilter Private Members
	//	Vector2f Radius;
	//	float Sigma, ExpX, ExpY;
	//	FilterSampler Sampler;
	//};

	// BoxFilter Definition
	class BoxFilter : public FilterBase
	{
	public:
		// BoxFilter Public Methods
		BoxFilter(const Vector2f &radius = Vector2f(0.5, 0.5)) : radius(radius) {}

		static BoxFilter *Create(float X, float Y)
		{
			return new BoxFilter({ X, Y });
		}

		Vector2f Radius() const { return radius; }


		float Evaluate(const Vector2f &p) const
		{
			return (std::abs(p.X) <= radius.X && std::abs(p.Y) <= radius.Y) ? 1 : 0;
		}

		FilterSample Sample(const Vector2f &u) const 
		{
			Vector2f p(Lerp( -radius.X, radius.X, u[0]), Lerp( -radius.Y, radius.Y, u[1]));
			return{ p, 1.f };
		}

		float Integral() const { return 2 * radius.X * 2 * radius.Y; }

	private:
		Vector2f radius;
	};



	#include "Filter.inl"
}