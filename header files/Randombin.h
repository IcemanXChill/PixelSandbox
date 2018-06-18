#pragma once
#include <random>

class Randombin
{
public:
	Randombin(int min, int max, int amount);
	int getnum();
private:
	std::default_random_engine m_engine;
	int max_numbers, min, max, counter;
	int* numberpool;

	std::uniform_int_distribution<int> dist;
};

