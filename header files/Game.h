#pragma once
#include <SFML/Graphics.hpp>
#include "Sandbox.h"
#include "Fps.h"
#include "KeyValues.h"


class Game
{
public:
	Game(KeyValue &settings);
	void run();

private:

	sf::RenderWindow mWindow;
	int state;
	int menu_choice = 0;
	FPS framecounter;

	//Various display functions placed here for each menu option.
private:
	int SCREEN_WIDTH, SCREEN_HEIGHT;
	void display_main_menu();
	void display_sandbox();

	//The main loop is setup here. (Engine related stuff.)
private:
	void process_events();
	void update();
	void render();
	void launch_option();

	//Init functions placed here.
	void initmenu();

	//Main menu objects placed here.
private:
	sf::Text T_singleplayer;
	sf::Text T_freemode;
	sf::Text T_multiplayer;
	sf::Text T_options;

	sf::Font F_menufont;

	//Sandbox free mode objects placed here.
private:
	Sandbox freemodebox;

	sf::Text FPStext;
};
