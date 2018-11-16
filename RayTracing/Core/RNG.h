#pragma once
class RNG
{
public:
	RNG(unsigned int seed = 5489UL);
	float GetRandomFloat();
	unsigned int GetRandomInt();
private:
};
