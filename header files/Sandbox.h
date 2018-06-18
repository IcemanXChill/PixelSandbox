#pragma once
#include <SFML\Graphics.hpp>
#include "Element.h"
#include <vector>
#include "Randombin.h"
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <locale>
#include <string>
#include <algorithm>
#include <SFML\Network.hpp>
#include <thread>
#include "FPacket.h"
#include "KeyValues.h"

const unsigned int MAX_ELEMENTS = 100;
const unsigned int MAX_GRIDSIZE_W = 1024;



//An enum for all the Element types.
enum ElementTypes { EMPTY, SOLID, LIQUID, POWDER, HEAT, GAS, STONE };

enum ReactionTypes { NORMAL, EXPLOSION, PIPE, TELEPORT };

enum Multiplayer { LOCAL, HOST, CLIENT };

enum MultiplayerAction { CREATE_ELEMENT, MOUSE_COORDS };

enum Directions { UP, LEFT, RIGHT, DOWN };

class Reaction
{
public:
	int result_a[MAX_ELEMENTS], result_b[MAX_ELEMENTS];
	double chance[MAX_ELEMENTS];
	int amt = 0;
	ReactionTypes type = NORMAL;
};

class Group
{
public:
	std::string name;
	std::vector<int> elements;
	int previous_element = 0;
	Group::Group()
		:elements()
	{
	}
};

class Sandbox
{
public:
	Sandbox(KeyValue &settings, sf::RenderWindow &owner_win ,bool mission);


public:
	void processgrid();

public:
	void draw_element_circle(int spx, int spy, int radius, int eid);

	bool moveable(sf::Vector2i pos1, sf::Vector2i pos2);

	void draw(sf::RenderWindow& win);

	void handle_event(sf::Event event);

	void check_events(sf::RenderWindow& win);

	void clear_sandbox(int eid);

	//Functions of the sandbox placed here.
private:
	bool mouseLdwn, mouseRdwn;
	int oldmousex,oldmousey;

	int scale;

	sf::RenderWindow* owner;

	void update(const int &x, const int &y);
	void do_self(int x, int y);
	void receive_action();
	void init_sandbox();
	void connect();

	void host();

	void project_action(sf::Uint32 pkgid, sf::Uint32 x, sf::Uint32 y, sf::Uint32 size, sf::Uint32 eid);

	//Functions of the internal element grid here.
	void create_element(const int &x, const int &y, const int &e);

	void move(sf::Vector2i pos1, sf::Vector2i pos2);

	void clear(int x, int y, int e = 0);

	bool check(int x, int y);

	int getrandom();

	int getrandom_c();

	int MouseX();

	int MouseY();

	int SMouseX();

	int SMouseY();

	int point2index(int x, int y);

	void setpoint(float x, float y, int r, int g, int b);

	int lookupid(std::string ename);

	void add_group(std::string name, int eid);

	int checkforgroup(std::string name);

	std::string lower_case(std::string word);

	void Draw_Line(float x1, float y1, float x2, float y2, int thickness, int eid);

	//Functions of the internal element loading system here.
	void add_element(std::string name, std::string group, int r, int g, int b, int density, int type, double life = -1,bool visible = true);
	void add_reaction(std::string elea, std::string eleb, std::string result_a, std::string result_b, double chance, ReactionTypes type, bool all);
	void init_elements();
	void addself(std::string elea, std::string eleb, int chance);
	void init_selfs();
	void react(int, int, int, int);
	void init_reactions();

	//Functions of the Internal GUI go here.
	void populateGUI();
	void refill_elementbox();
	void on_element_selection();

	void on_group_selection();

	void on_size_slider_change();

	void on_ground_toggle();

	void on_pause_toggle();

	//Parts of the sandbox itself are stored here.
private:
	bool paused = false;


	//The graphics sprite, the graphics texture and the underlying pixel grid, in that order.
	sf::Sprite* display_image;

	sf::Texture sandboxtex;

	sf::Uint8* pixels;

	//Random generators go here.
	Randombin lr_chance;
	Randombin r_chance;
	Randombin gas_chance;

	//The underlying elements grid & its reactions.
	std::vector< std::vector<int> > elegrid;

	std::vector< Group > group_list;

	std::vector< std::vector<Reaction> > reaction_index;

	//The element table. Very important. ;)
	Element eletable[MAX_ELEMENTS];

	//The counter for how many elements this sandbox currently has.
	unsigned int element_counter = 0;

	int self_counter[MAX_ELEMENTS];
	int self_index[MAX_ELEMENTS][MAX_ELEMENTS];
	int self_chance_index[MAX_ELEMENTS][MAX_ELEMENTS];

	//Stats about the grid.
	unsigned int grid_width;
	unsigned int grid_height;

	int current_element = 1;
	int pen_size = 10;

	bool ground = false;

	//Networking stuff.

	sf::TcpSocket socket;

	sf::TcpListener listener;

	int multiplayer_status = LOCAL;

private:

	//GUI stuff.

		//Objects themselves.
	sfg::SFGUI m_gui;

	sfg::ComboBox::Ptr group_selector;

	sfg::ComboBox::Ptr element_selector;

	sfg::Window::Ptr toolbar;

	sfg::Box::Ptr hbox;

	sfg::Label::Ptr pen_size_label;

	sfg::Scale::Ptr size_slider;

	sfg::Adjustment::Ptr size_adjustment;

	sfg::Desktop* desktop;

	bool windowclick = false;

	sf::CircleShape pen_cursor;

	//Functions for the GUI objects.

	//Simple directional functions that return the direction they are named after.

	sf::Vector2i decide_LR(sf::Vector2i pos);

	sf::Vector2i decide_DL_DR(sf::Vector2i pos);

	sf::Vector2i above(sf::Vector2i pos);

	sf::Vector2i aboveleft(sf::Vector2i pos);

	sf::Vector2i aboveright(sf::Vector2i pos);

	sf::Vector2i left(sf::Vector2i pos);

	sf::Vector2i right(sf::Vector2i pos);

	sf::Vector2i below(sf::Vector2i pos);

	sf::Vector2i belowleft(sf::Vector2i pos);

	sf::Vector2i belowright(sf::Vector2i pos);

	sf::Vector2i watermove(sf::Vector2i pos);

	sf::Vector2i powdermove(sf::Vector2i pos);

	sf::Vector2i stonemove(sf::Vector2i pos);

	sf::Vector2i firemove(sf::Vector2i pos);

	sf::Vector2i gasmove(sf::Vector2i pos);
};
