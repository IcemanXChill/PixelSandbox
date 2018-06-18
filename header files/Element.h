#pragma once
#include <string>

class Element
{
public:
	Element();
	int r, g, b;
	int id, type, density;
	double life;
	int reactions_num = 0, self_num = 0;
	std::string name;
	bool reacts = false;
	bool visible = true;
};
