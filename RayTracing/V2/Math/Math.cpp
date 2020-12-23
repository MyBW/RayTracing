#include "Math.hpp"
#include <cmath>
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
