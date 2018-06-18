#pragma once
#include <vector>
#include "Element.h"

class ElementLoader
{
public:
	ElementLoader();
	~ElementLoader();

	std::vector<Element> get_element_list();

	void load_from_file(std::string filename);

	void load_default_file();


private:

	void add_element(std::string name, std::string group, int r, int g, int b, int density, int type, double life = -1, bool visible = true);



};

