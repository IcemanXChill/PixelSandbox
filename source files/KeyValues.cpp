#include "KeyValues.h"

int KeyValue::GetInt(std::string id)
{
	if (id == "Width")
		return 640;
	if (id == "Height")
		return 480;
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