#pragma once
namespace BlackWalnut
{
	class Ray
	{
	public:
		Ray() = default;
		Ray(const Vector3f& O, const Vector3f& D, float Time = 0) :O(O),D(D),Time(Time)
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
		RayDifferential(const Vector3f& O, cosnt Vector3f& D, float Time) :Ray(O, D, Time)
		{

		}
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
}