#pragma once
class RNG
{
public:
	RNG(unsigned int seed = 5489UL);
	void Seed(unsigned int seed) const;
	float GetRandomFloat();
	unsigned int GetRandomUInt();
private:
	static const int N = 624;
	mutable unsigned long mt[N]; /* the array for the state vector  */
	mutable int mti;
};
