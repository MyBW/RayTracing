#include "Shape.hpp"
namespace BlackWalnut
{

	BlackWalnut::Vector3f ShapeSampleContext::OffsetRayOrigin(const Vector3f &w) const
	{
		//// Find vector _offset_ to corner of error bounds and compute initial _po_
		//float d = Dot(Abs(n), pi.Error());
		//Vector3f offset = d * Vector3f(n);
		//if (Dot(w, n) < 0)
		//	offset = -offset;
		//Vector3f po = pi + offset;

		//// Round offset point _po_ away from _p_
		//for (int i = 0; i < 3; ++i) {
		//	if (offset[i] > 0)
		//		po[i] = NextFloatUp(po[i]);
		//	else if (offset[i] < 0)
		//		po[i] = NextFloatDown(po[i]);
		//}

		//return po;
		return Vector3f(0.0f);
	}

	Ray ShapeSampleContext::SpawnRay(const Vector3f &w) const
	{
		// Note: doesn't set medium, but that's fine, since this is only
		// used by shapes to see if ray would have intersected them
		return Ray(OffsetRayOrigin(w), w, time);
	}
}


