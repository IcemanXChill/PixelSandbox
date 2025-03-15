//#include <SFGUI/SFGUI.hpp>
//#include <SFGUI/Widgets.hpp>
#include "Fps.h"
#include "Game.h"

KeyValue settings_file;

void perform_init()
{

		const int SCREEN_WIDTH = settings_file.GetInt("Width");
		const int SCREEN_HEIGHT = settings_file.GetInt("Height");
		const int Scale = settings_file.GetInt("Scale");
	



}


int main() {


	perform_init();

	Game mygame(settings_file);

	mygame.run();

	return 0;
}