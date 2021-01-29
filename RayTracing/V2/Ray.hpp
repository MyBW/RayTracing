#pragma once
#include "Math\Geom.hpp"
namespace BlackWalnut
{
	class Ray
	{
	public:
		Ray() = default;
		Ray(const Vector3f& O, const Vector3f& D, float Time = 0) :O(O), D(D), Time(Time)
		{

		}
		bool HasNaN() const
		{
			return (O.IsNaN() || D.IsNaN());
		}
		Vector3f operator()(float t) const { return O + D*t; }
		Vector3f O;
		Vector3f D;
		float Time = 0;
	};

	class RayDifferential : public Ray
	{
	public:
		RayDifferential(const Vector3f& O, const Vector3f &D, float Time) :Ray(O, D, Time)
		{

		}
		RayDifferential(const Ray &InRay) :Ray(InRay) { }
		void ScaleDifferentials(float S)
		{
			RxOrigin = O + (RxOrigin - O) * S;
			RyOrigin = O + (RyOrigin - O) * S;
			RxDirection = D + (RxDirection - D) *S;
			RyDirection = D + (RyDirection - D) *S;
		}
		bool HasNaN() const
		{
			return Ray::HasNaN() && (HasDifferentials && (RxDirection.IsNaN() || RyDirection.IsNaN() || RxOrigin.IsNaN() || RyOrigin.IsNaN()));
		}

		bool HasDifferentials = false;
		Vector3f RxOrigin, RyOrigin;
		Vector3f RxDirection, RyDirection;
	};
	inline Vector3f OffsetRayOrigin(Vector3f Pi, Vector3f Normal, Vector3f W)
	{
		//float D = Dot(Abs(Normal), )
		return Pi + W * 0.0001; // this is not same in PBRT
	}
	inline Ray SpawnRay(Vector3f Pi, Vector3f Noraml, float Time, Vector3f D)
	{
		return Ray(OffsetRayOrigin(Pi, Noraml, D), D, Time);
	}
	inline Ray SpawnRayTo(Vector3f PFrom, Vector3f Normal, float Time, Vector3f PTo)
	{
		Vector3f d = PTo - PFrom;
		return SpawnRay(PFrom, Normal, Time, d);
	}
	inline Ray SpawnRayTo(Vector3f PFrom, Vector3f Normal, float Time, Vector3f PTo, Vector3f ToNormal)
	{
		Vector3f PF = OffsetRayOrigin(PFrom, Normal, PTo - PFrom);
		Vector3f PT = OffsetRayOrigin(PTo, ToNormal, PF - PTo);
		return Ray(PF, PT - PF, Time);
	}
}