#include "RNG.h"
#include <ctime>
#include <random>
RNG::RNG(unsigned int seed /*= 5489UL*/)
{
	srand(int(time(0)));
}
float RNG::GetRandomFloat()
{
	return float(rand()) / RAND_MAX;
}

unsigned int RNG::GetRandomInt()
{
	return rand();
}
