#include "Game.h"




enum states
{
	MAIN_MENU,
	SINGLE_PLAYER,
	MULTIPLAYER,
	FREE_MODE,
	OPTIONS
};

void Game::initmenu()
{
	state = FREE_MODE;
	F_menufont.loadFromFile("arial.ttf");

	T_singleplayer.setFont(F_menufont);
	T_freemode.setFont(F_menufont);
	T_multiplayer.setFont(F_menufont);
	T_options.setFont(F_menufont);

	T_singleplayer.setString(sf::String("Alchemy Mode"));
	T_freemode.setString(sf::String("Sandbox"));
	T_multiplayer.setString(sf::String("Multi Player"));
	T_options.setString(sf::String("Options"));

/*	T_singleplayer.setOrigin(T_singleplayer.getLocalBounds().width / 2, T_singleplayer.getLocalBounds().height / 2);
	T_freemode.setOrigin(T_freemode.getLocalBounds().width / 2, T_freemode.getLocalBounds().height / 2);
	T_multiplayer.setOrigin(T_multiplayer.getLocalBounds().width / 2, T_multiplayer.getLocalBounds().height / 2);
	T_options.setOrigin(T_options.getLocalBounds().width / 2, T_options.getLocalBounds().height / 2);

	T_singleplayer.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100);
	T_freemode.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	T_multiplayer.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100);
	T_options.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 200);
	*/
}

Game::Game(KeyValue &settings)
	:mWindow(sf::VideoMode(settings.GetInt("Width"), settings.GetInt("Height")), "Sandbox Game", sf::Style::Close | sf::Style::Titlebar)
	, freemodebox(settings, mWindow, true)
{
	SCREEN_WIDTH = settings.GetInt("Width");
	SCREEN_HEIGHT = settings.GetInt("Height");
	initmenu();
	FPStext.setFont(F_menufont);
	FPStext.setPosition(sf::Vector2f(0, 50));
	mWindow.setVerticalSyncEnabled(false);
	mWindow.setFramerateLimit(settings.GetInt("Framelimit"));
}

void Game::display_main_menu()
{
	T_singleplayer.setFillColor(sf::Color(128, 128, 128, 255));
	T_freemode.setFillColor(sf::Color(128, 128, 128, 255));
	T_multiplayer.setFillColor(sf::Color(128, 128, 128, 255));
	T_options.setFillColor(sf::Color(128, 128, 128, 255));

	if (menu_choice == 0)
		T_singleplayer.setFillColor(sf::Color(255, 255, 255, 255));
	if (menu_choice == 1)
		T_freemode.setFillColor(sf::Color(255, 255, 255, 255));
	if (menu_choice == 2)
		T_multiplayer.setFillColor(sf::Color(255, 255, 255, 255));
	if (menu_choice == 3)
		T_options.setFillColor(sf::Color(255, 255, 255, 255));

	mWindow.draw(T_singleplayer);
	mWindow.draw(T_freemode);
	mWindow.draw(T_multiplayer);
	mWindow.draw(T_options);
}

void Game::display_sandbox()
{
	freemodebox.draw(mWindow);
}

void Game::process_events()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		//No matter what, we should always be able to close the window.
		if (event.type == sf::Event::Closed)
			mWindow.close();

		switch (state)
		{
		case MAIN_MENU:
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Up)
					menu_choice--;
				if (menu_choice < 0)
					menu_choice = 3;

				if (event.key.code == sf::Keyboard::Down)
					menu_choice++;
				if (menu_choice > 3)
					menu_choice = 0;

				if (event.key.code == sf::Keyboard::Return)
					launch_option();
				if (event.key.code == sf::Keyboard::Escape)
					mWindow.close();

				if (event.key.code == sf::Keyboard::F)
					mWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Sandbox Game", sf::Style::Fullscreen);
			}
			break;

		case FREE_MODE:
			freemodebox.handle_event(event);
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
					state = MAIN_MENU;
			}
			break;

		default:
			break;
		}
	}

	switch (state)
	{
	case FREE_MODE:
		freemodebox.check_events(mWindow);
		break;
	default:
		break;
	}
}

void Game::update()
{
	switch (state)
	{
	case MAIN_MENU:
		//Do stuff here.
		break;

	case FREE_MODE:
		freemodebox.processgrid();
		break;

	default:
		break;
	}
}

void Game::render()
{
	//Clear the window at start, draw everything all over, and show it again.
	mWindow.clear();

	//Here I choose what to draw.
	switch (state)
	{
	case MAIN_MENU:
		display_main_menu();
		break;

	case FREE_MODE:
		display_sandbox();

		break;

	default:
		break;
	}

	//This is the debug framecounter.
	FPStext.setString(std::to_string(framecounter.getFPS()));
	mWindow.draw(FPStext);

	//No matter what, we draw the main window.
	mWindow.display();
}

void Game::launch_option()
{
	switch (menu_choice)
	{
	case 1:
		state = FREE_MODE;
		break;

	default:
		break;
	}
}

void Game::run()
{
	while (mWindow.isOpen())
	{
		process_events();
		update();

		framecounter.update();
		render();
	}
}