
BWVector3D ConsineSampleHemisphere(float U1, float U2)
{
	BWVector3D Ret;
	ConcentricSampleDisk(U1, U2, &Ret.x, &Ret.y);
	Ret.z = sqrtf(max(0.f, 1.f - Ret.x*Ret.x - Ret.y*Ret.y));
	return Ret;
}

