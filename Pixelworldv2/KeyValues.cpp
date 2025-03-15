#include "KeyValues.h"

int KeyValue::GetInt(std::string id)
{
	if (id == "Width")
		return 1920;
	if (id == "Height")
		return 1080;
	if (id == "Scale")
		return 4;
	if (id == "Framelimit")
		return 300;
	else return 0;
}

/*
settings_file.SetInt("Width", 1920);
settings_file.SetInt("Height", 1080);
settings_file.SetInt("Scale", 4);

*/