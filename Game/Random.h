#pragma once

class Random
{
private:
	unsigned long ulState[16];
	unsigned long ulSeed;
	unsigned int iIndex;

public:
	static Random rand;

	Random();
	Random(const unsigned long seed);

	unsigned long Next();
	double NextDouble();

	unsigned long GetSeed() const;
	void SetSeed(unsigned long seed);
	void ResetSeed();
};