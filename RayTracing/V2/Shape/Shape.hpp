#pragma once
#include "..\V2\Math\Geom.hpp"
#include "..\V2\Interaction.hpp"
namespace BlackWalnut
{
	// ShapeSample Definition
	struct ShapeSample {
		Interaction intr;
		float pdf;
	};
	struct ShapeSampleContext {
		// ShapeSampleContext Public Methods
		ShapeSampleContext() = default;
		
		ShapeSampleContext(const Vector3f &pi, const Vector3f &n, const Vector3f &ns,
				float time)
			: pi(pi), n(n), ns(ns), time(time) {}
		
		ShapeSampleContext(const SurfaceInteraction &si)
			: pi(si.pi), n(si.n), ns(si.shading.n), time(si.time) {}
		
		Vector3f p() const { return pi; }

		
		Vector3f OffsetRayOrigin(const Vector3f &w) const;
		
		Ray SpawnRay(const Vector3f &w) const;

		Vector3f pi;
		Vector3f n, ns;
		float time;
	};

	class ShapeBase
	{
	public:
		ShapeBase(const Matrix4X4f &renderFromObject, const Matrix4X4f &objectFromRender)
			:RenderFromObject(renderFromObject), ObjectFromRender(objectFromRender)
		{

		}
		virtual ShapeSample Sample(const Vector2f &u) const = 0;
		virtual ShapeSample Sample(const ShapeSampleContext &ctx, const Vector2f &u) const = 0;
		virtual float Area() const = 0;
		virtual float PDF(const ShapeSampleContext &ctx, const Vector3f &wi) const = 0;
	protected:
		Matrix4X4f RenderFromObject, ObjectFromRender;
	};
	class DiskShape : public ShapeBase
	{
	public:
		DiskShape(const Matrix4X4f &renderFromObject, const Matrix4X4f &objectFromRender,
			bool reverseOrientation, float height, float radius, float innerRadius,
			float phiMax):ShapeBase(renderFromObject, objectFromRender),
			ReverseOrientation(reverseOrientation),
			height(height),
			radius(radius),
			innerRadius(innerRadius),
			phiMax(Radians(Clamp(phiMax, 0, 360))) {}
		ShapeSample Sample(const Vector2f &u) const override
		{
			Vector2f pd = SampleUniformDiskConcentric(u);
			//Vector3f pObj(pd.X * radius, pd.Y * radius, height); //pbrt 是z轴为高度 这里用Y轴为高度
			Vector3f pObj(pd.X * radius,  height, pd.Y * radius); //pbrt 是z轴为高度 这里用Y轴为高度
			Vector3f pi;
			Matrix4X4MultVector3(pi, RenderFromObject, pObj);
			Vector3f n;
			Matrix4X4MultVector3(n, RenderFromObject, Vector3f(0, 0, -1), 0.0f);
			Normalize(n);
			if (ReverseOrientation)
				n = n * (-1);
			return ShapeSample{ Interaction(pi, n), 1 / Area() };
		}
		ShapeSample Sample(const ShapeSampleContext &ctx, const Vector2f &u) const override
		{
			// Uniformly sample shape and compute incident direction _wi_
			ShapeSample ss = Sample(u);
			ss.intr.time = ctx.time;
			Vector3f wi = ss.intr.P() - ctx.p();
			if (Length(wi) == 0)
				return{};
			Normalize(wi);

			// Convert uniform area sample PDF in _ss_ to solid angle measure
			ss.pdf /= AbsDot(ss.intr.n, -wi) / DistanceSquared(ctx.p(), ss.intr.P());
			if (IsInf(ss.pdf))
				return{};

			return ss;
		}
		float Area() const { return phiMax * 0.5f * (radius * radius - innerRadius* innerRadius); }
		float PDF(const ShapeSampleContext &ctx, const Vector3f &wi) const override
		{
			return 0;
		}
	private:
		
		bool ReverseOrientation;
		float height, radius, innerRadius, phiMax;
	};
}