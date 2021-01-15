#include "Math.hpp"
#include <cmath>
#define PI 3.14159265358979323846f
int BlackWalnut::DivideAndRoundUp(int Dividend, int Divisor)
{
	return (Dividend + Divisor - 1) / Divisor;
}

float BlackWalnut::Exp(float Data)
{
	return expf(Data);
}

float BlackWalnut::Sqrt(float Data)
{
	return sqrtf(Data);
}

float BlackWalnut::Loge(float Data)
{
	return logf(Data);
}

float BlackWalnut::Sin(float Data)
{
	return sinf(Data);
}

float BlackWalnut::Cos(float Data)
{
	return cosf(Data);
}

float BlackWalnut::Pow(float A, float B)
{
	return pow(A, B);
}

int BlackWalnut::CeilToInt(float F)
{
	return F - int(F) > 0 ? int(F) + 1 : int(F);
}
float BlackWalnut::Gaussian(float x, float mu /* = 0 */, float sigma /* = 1 */)
{
	return 1 / std::sqrt(2 * PI * sigma * sigma) *
		Exp(-std::sqrt(x - mu) / (2 * sigma * sigma));
}
