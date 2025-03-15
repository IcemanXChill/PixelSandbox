#include "Sandbox.h"

//Init function for the sandbox here.
Sandbox::Sandbox(KeyValue& settings, sf::RenderWindow& owner_win, bool mission = false)
	:elegrid(settings.GetInt("Width") / settings.GetInt("Scale"), std::vector<int>(settings.GetInt("Height") / settings.GetInt("Scale"))),
	group_list(),
	reaction_index(MAX_ELEMENTS, std::vector<Reaction>(MAX_ELEMENTS)),
	r_chance(0, 1000, 20000),
	lr_chance(1, 4, 2000),
	gas_chance(1, 4, 100),
	pen_cursor(pen_size)
{
	scale = settings.GetInt("Scale");

	owner = &owner_win;

	sandboxtex.create(settings.GetInt("Width") / scale, settings.GetInt("Height") / scale);

	display_image = new sf::Sprite(sandboxtex);

	display_image->scale(scale, scale);

	pixels = new sf::Uint8[settings.GetInt("Width") / scale * settings.GetInt("Height") / scale * 4];

	init_elements();

	init_reactions();

	init_selfs();

	grid_width = settings.GetInt("Width") / scale;
	grid_height = settings.GetInt("Height") / scale;

	init_sandbox();

	pen_cursor.setFillColor(sf::Color::Transparent);
	pen_cursor.setOutlineThickness(1);
	pen_cursor.setOrigin(pen_size, pen_size);

	//Networking stuff here.
	socket.setBlocking(false);

	//GUI stuff here.
	auto ground_checkbox = sfg::CheckButton::Create("Floor");
	ground_checkbox->GetSignal(sfg::CheckButton::OnToggle).Connect(std::bind(&Sandbox::on_ground_toggle, this));

	auto pause_checkbox = sfg::CheckButton::Create("Pause");
	pause_checkbox->GetSignal(sfg::CheckButton::OnToggle).Connect(std::bind(&Sandbox::on_pause_toggle, this));
	pause_checkbox->SetRequisition(sf::Vector2f(1, 1));

	// In the Sandbox constructor, after creating pause_checkbox:
	auto clear_button = sfg::Button::Create("Clear");
	clear_button->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind(&Sandbox::on_clear_button_click, this));


	size_slider = sfg::Scale::Create(1, 100, 1);
	size_slider->GetAdjustment()->GetSignal(sfg::Adjustment::OnChange).Connect(std::bind(&Sandbox::on_size_slider_change, this));

	pen_size_label = sfg::Label::Create("Pen Size: " + std::to_string(pen_size));
	pen_size_label->SetRequisition(sf::Vector2f(100.0, 0.0));

	element_selector = sfg::ComboBox::Create();
	element_selector->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&Sandbox::on_element_selection, this));

	group_selector = sfg::ComboBox::Create();
	group_selector->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&Sandbox::on_group_selection, this));

	auto vbox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5);
	vbox->Pack(ground_checkbox);
	vbox->Pack(pause_checkbox);
	vbox->Pack(clear_button);

	hbox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5);
	hbox->Pack(pen_size_label, false);
	hbox->Pack(size_slider);
	hbox->Pack(element_selector);
	hbox->Pack(group_selector);
	hbox->Pack(vbox);

	size_slider->SetIncrements(scale, scale);

	toolbar = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
	toolbar->SetRequisition(sf::Vector2f(settings.GetInt("Width"), 10));
	toolbar->Add(hbox);


	desktop = new sfg::Desktop;
	desktop->Add(toolbar);
	desktop->SetProperty("group_selector", "Color", sf::Color::Blue);

	populateGUI();
	group_selector->SelectItem(0);
	//element_selector->SelectItem(1);
	refill_elementbox();


	if (mission)
	{
		ground_checkbox->SetActive(true);
		ground = true;

	}

}

//Functions called from outside the sandbox go here.
void Sandbox::draw(sf::RenderWindow& win)
{

	sandboxtex.update(pixels);
	win.draw(*display_image);
	pen_cursor.setPosition(sf::Mouse::getPosition(win).x, sf::Mouse::getPosition(win).y);
	win.draw(pen_cursor);
	desktop->Update(10);
	m_gui.Display(win);
}

void Sandbox::handle_event(sf::Event event)
{
	desktop->HandleEvent(event);
	if (event.type == sf::Event::MouseWheelMoved)
	{
		pen_size += event.mouseWheel.delta;
		if (pen_size <= 0)
			pen_size = 1;
		pen_cursor.setRadius(pen_size);
		pen_cursor.setOrigin(pen_size, pen_size);
		pen_size_label->SetText("Pen Size: " + std::to_string(pen_size));
		size_slider->SetValue(pen_size);
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::C)
		{
			connect();
			multiplayer_status = CLIENT;
		}

		if (event.key.code == sf::Keyboard::H)
		{
			host();
			multiplayer_status = HOST;
		}
	}

	if (event.type == sf::Event::MouseButtonPressed && !windowclick && !toolbar->GetAllocation().contains(sf::Vector2f(sf::Mouse::getPosition(*owner).x, sf::Mouse::getPosition(*owner).y)))
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{

			mouseLdwn = true;
			oldmousex = MouseX();
			oldmousey = MouseY();

		}

		if (event.mouseButton.button == sf::Mouse::Right)
		{

			mouseRdwn = true;
			oldmousex = MouseX();
			oldmousey = MouseY();

		}

	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{

			mouseLdwn = false;

		}

		if (event.mouseButton.button == sf::Mouse::Right)
		{

			mouseRdwn = false;

		}

	}

	if (event.type == sf::Event::MouseMoved)
	{

		if (mouseLdwn)
		{
			Draw_Line(oldmousex, oldmousey, MouseX(), MouseY(), pen_size / scale, current_element);
			oldmousex = MouseX();
			oldmousey = MouseY();
		}

		if (mouseRdwn)
		{
			Draw_Line(oldmousex, oldmousey, MouseX(), MouseY(), pen_size / scale, 0);
			oldmousex = MouseX();
			oldmousey = MouseY();
		}


	}

}

void Sandbox::check_events(sf::RenderWindow& win)
{
	if (mouseLdwn)
		draw_element_circle(SMouseX() + 1, SMouseY() + 1, pen_size / scale, current_element);

	if (mouseRdwn)
		draw_element_circle(SMouseX(), SMouseY(), pen_size / scale, 0);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && toolbar->GetAllocation().contains(sf::Vector2f(sf::Mouse::getPosition(win).x, sf::Mouse::getPosition(win).y)))
		windowclick = true;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !toolbar->GetAllocation().contains(sf::Vector2f(sf::Mouse::getPosition(win).x, sf::Mouse::getPosition(win).y)))
		draw_element_circle(SMouseX(), SMouseY(), pen_size / scale, 0);
	if (multiplayer_status != LOCAL)
	{
		project_action(MultiplayerAction::CREATE_ELEMENT, sf::Mouse::getPosition(win).x, sf::Mouse::getPosition(win).y, pen_size, 0);
	}

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
		windowclick = false;
}

void Sandbox::clear_sandbox(int eid)
{
	for (int x = 0; x < grid_width; x++)
	{
		for (int y = 0; y < grid_height; y++)
		{
			create_element(x, y, eid);
		}
	}
}

//Grid actions here.
bool Sandbox::check(int x, int y)
{
	if (x > 0 && y > 0 && y < grid_height && x < grid_width)

		return true;

	else

		return false;
}

int Sandbox::point2index(int x, int y)
{
	return ((y - 1) * grid_width) + (x - 1);
}

void Sandbox::setpoint(float x, float y, int r, int g, int b)
{
	if (check(x, y))
	{
		int index = point2index(x, y) * 4;

		pixels[index] = r;
		pixels[index + 1] = g;
		pixels[index + 2] = b;
		pixels[index + 3] = 255;

	}
}

bool Sandbox::moveable(sf::Vector2i pos1, sf::Vector2i pos2)
{
	if (check(pos2.x, pos2.y))

		if (eletable[elegrid[pos2.x][pos2.y]].type == 0 || eletable[elegrid[pos1.x][pos1.y]].density > eletable[elegrid[pos2.x][pos2.y]].density && eletable[elegrid[pos2.x][pos2.y]].density != 0)
			return true;
		else
			return false;

	else

		return false;
}

void Sandbox::draw_element_circle(int spx, int spy, int radius, int eid)
{
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			if (x * x + y * y <= radius * radius)
			{
				if (check(spx, spy))
					create_element(spx + x, spy + y, eid);
			}
		}
	}
}

void Sandbox::move(sf::Vector2i pos1, sf::Vector2i pos2)
{
	int e = elegrid[pos1.x][pos1.y];
	create_element(pos1.x, pos1.y, elegrid[pos2.x][pos2.y]);
	create_element(pos2.x, pos2.y, e);
}

void Sandbox::create_element(const int& x, const int& y, const int& e)
{
	if (check(x, y))
	{
		setpoint(x, y, eletable[e].r, eletable[e].g, eletable[e].b);
		elegrid[x][y] = e;
	}
}

void Sandbox::update(const int& x, const int& y)
{
	if (eletable[elegrid[x][y]].reacts)
	{
		sf::Vector2i pos(x, y);
		react(x, y, above(pos).x, above(pos).y);
		react(x, y, left(pos).x, left(pos).y);
		react(x, y, right(pos).x, right(pos).y);
		react(x, y, below(pos).x, below(pos).y);
	}

	if (eletable[elegrid[x][y]].type != SOLID)
	{
		sf::Vector2i pos(x, y);
		sf::Vector2i movespot;
		switch (eletable[elegrid[x][y]].type)
		{
		case LIQUID:
			movespot = watermove(pos);
			break;
		case POWDER:
			movespot = powdermove(pos);
			break;
		case HEAT:
			movespot = firemove(pos);
			break;
		case GAS:
			movespot = gasmove(pos);
			break;
		case STONE:
			movespot = stonemove(pos);
			break;
		}
		if (pos != movespot)
			move(pos, movespot);
	}
}

void Sandbox::processgrid()
{
	receive_action();

	if (!paused)
	{
		for (int x = 0; x < grid_width; x++)
		{
			for (int y = grid_height - 1; y > 0; y--)
			{
				if (eletable[elegrid[x][y]].type > 0)
				{
					if (y > grid_height - 2 && ground == false)
					{
						clear(x, y);
					}
					do_self(x, y);
					update(x, y);
				}
			}
		}
	}
}

void Sandbox::do_self(int x, int y)
{
	int e = elegrid[x][y];
	int amt = self_counter[e];
	if (amt > 0)
	{
		for (int ec = 0; ec < amt; ec++)
		{
			if (getrandom_c() < self_chance_index[e][ec])
			{
				create_element(x, y, self_index[e][ec]);
				break;
			}
		}
	}
}

int Sandbox::lookupid(std::string ename)
{
	for (int i = 0; i < element_counter; i++)
	{
		if (eletable[i].name == ename) {
			return i;
		}
	}
}

void Sandbox::connect()
{
	//socket.connect(sf::IpAddress("68.170.252.55"), 27015);
	socket.connect(sf::IpAddress::getLocalAddress(), 27015);
}

void Sandbox::host()
{
	listener.listen(27015);
	listener.accept(socket);
}

void Sandbox::project_action(sf::Uint32 pkgid, sf::Uint32 x, sf::Uint32 y, sf::Uint32 size, sf::Uint32 eid)
{
	sf::Packet testpacket;

	switch (pkgid)
	{
	case MultiplayerAction::CREATE_ELEMENT:
		testpacket << pkgid << x << y << size << eid;
		break;
	case MultiplayerAction::MOUSE_COORDS:
		testpacket << pkgid << x << y;
		break;
	}


	socket.send(testpacket);
}

void Sandbox::receive_action()
{

	sf::Packet testpacket;
	sf::Uint32 pkgid;

	socket.receive(testpacket);

	testpacket >> pkgid;

	switch (pkgid)
	{
		sf::Uint32 x, y, eid, size;
	case MultiplayerAction::CREATE_ELEMENT:
		testpacket >> x >> y >> size >> eid;
		draw_element_circle(x, y, size, eid);
		break;
	case MultiplayerAction::MOUSE_COORDS:
		testpacket >> x >> y;
		break;
	}

	testpacket.clear();

}

void Sandbox::init_sandbox()
{
	for (int x = 0; x < grid_width; x++)
	{
		for (int y = 0; y < grid_height; y++)
		{
			create_element(x, y, 0);
		}
	}
}

void Sandbox::clear(int x, int y, int e)
{
	create_element(x, y, e);
}

//Internal loading/creating goes here. Add element, group, etc.

void Sandbox::add_group(std::string name, int eid)
{
	int id = checkforgroup(name);
	if (id > -1)
	{
		group_list[id].elements.push_back(eid);
	}
	else
	{
		Group g;
		g.name = name;
		g.elements.push_back(eid);
		group_list.push_back(g);
	}
}

int Sandbox::checkforgroup(std::string name)
{
	for (int i = 0; i < group_list.size(); i++)
	{
		if (group_list[i].name == name)
		{
			return i;
		}
	}
	return -1;
}

void Sandbox::add_element(std::string name, std::string group, int r, int g, int b, int density, int type, double life, bool visible)
{
	eletable[element_counter].name = name;
	eletable[element_counter].r = r;
	eletable[element_counter].g = g;
	eletable[element_counter].b = b;
	eletable[element_counter].density = density;
	eletable[element_counter].type = type;
	eletable[element_counter].life = life;
	eletable[element_counter].id = element_counter;
	eletable[element_counter].visible = visible;
	add_group(group, element_counter);
	element_counter++;
}

void Sandbox::add_reaction(std::string elea, std::string eleb, std::string result_a, std::string result_b,
	double chance, ReactionTypes type, bool all, DirectionFlags dirs)
{
	int ida = lookupid(elea);
	int idb = lookupid(eleb);

	int idresa = lookupid(result_a);
	int idresb = lookupid(result_b);

	reaction_index[ida][idb].result_a[reaction_index[ida][idb].amt] = eletable[idresa].id;
	reaction_index[ida][idb].result_b[reaction_index[ida][idb].amt] = eletable[idresb].id;
	reaction_index[ida][idb].chance[reaction_index[ida][idb].amt] = chance;
	reaction_index[ida][idb].directions[reaction_index[ida][idb].amt] = dirs; // Store direction flags
	reaction_index[ida][idb].type = type; // Fixed assignment (was using == comparison)

	if (all)
	{
		for (int i = 1; i < element_counter; i++)
		{
			if (ida != i)
			{
				add_reaction(elea, eletable[i].name, result_a, result_b, chance, type, false, dirs);
			}
		}
	}

	eletable[ida].reacts = true;
	reaction_index[ida][idb].amt++;
}

void Sandbox::addself(std::string elea, std::string eleb, int chance)
{
	int ele_a = lookupid(elea);
	int ele_b = lookupid(eleb);
	int amt = self_counter[ele_a];

	self_index[ele_a][amt] = ele_b;
	self_chance_index[ele_a][amt] = chance;
	self_counter[ele_a]++;
}

//Default Behaviors loaded here.
void Sandbox::init_elements()
{
	add_element("Air", "Tools", 0, 0, 0, 0, EMPTY, -1, true);
	add_element("Wall", "Tools", 128, 128, 128, 100, SOLID, -1, true);
	add_element("Water", "Liquids", 0, 0, 255, 3, LIQUID, -1, true);
	add_element("Sprinkler", "Spouts", 255, 255, 255, 0, SOLID, -1, true);
	add_element("Torch", "Spouts", 170, 69, 19, 0, SOLID, -1, true);
	add_element("Oil Well", "Spouts", 100, 69, 19, 0, SOLID, -1, true);
	add_element("Salt", "Powders", 255, 255, 255, 4, LIQUID, -1, true);
	add_element("Oil", "Liquids", 54, 46, 32, 4, LIQUID, -1, true);
	add_element("Acid", "Liquids", 128, 255, 128, 0, LIQUID, -1, true);
	add_element("Gas", "Gases", 80, 80, 80, 0, GAS, -1, true);
	add_element("Sand", "Powders", 237, 201, 175, 5, POWDER, -1, true);
	add_element("Gravel", "Powders", 180, 180, 180, 5, STONE, -1, true);
	add_element("Dirt", "Powders", 139, 69, 19, 5, STONE, -1, true);
	add_element("Soil", "Powders", 100, 30, 10, 5, STONE, -1, true);
	add_element("Plant", "Nature", 0, 255, 0, 0, SOLID, -1, true);
	add_element("Smoke", "Nature", 60, 60, 60, 100, HEAT, -1, true);
	add_element("Fire", "Nature", 255, 100, 100, 100, HEAT, -1, true);
	add_element("Wood", "Nature", 139, 69, 16, 100, SOLID, -1, true);
	add_element("Sponge", "Powders", 255, 255, 0, 4, LIQUID, -1, true);
	add_element("Skin", "Surgery", 230, 200, 170, 100, SOLID, -1, true);
	add_element("DeadSkin", "Surgery", 130, 100, 70, 100, SOLID, -1, true);
	add_element("Skull", "Surgery", 255, 255, 255, 100, SOLID, -1, true);
	add_element("Brain", "Surgery", 255, 192, 203, 100, SOLID, -1, true);
	add_element("Wound Cleaner", "Surgery", 255, 255, 255, 100, LIQUID, -1, true);
	add_element("Scapel", "Surgery", 0, 255, 0, 100, SOLID, -1, true);
	add_element("Blood", "Surgery", 255, 0, 0, 4, LIQUID, -1, true);
	add_element("Wound", "Surgery", 130, 70, 15, 100, SOLID, -1, true);
	add_element("Sun", "Nature", 255, 255, 0, 100, SOLID, -1, true);
	add_element("Darter", "Tools", 255, 140, 0, 1, SOLID, -1, true);
	add_element("DarterU", "Tools", 255, 140, 0, 1, SOLID, -1, true);
	add_element("DarterD", "Tools", 255, 140, 0, 1, SOLID, -1, true);
	add_element("DarterL", "Tools", 255, 140, 0, 1, SOLID, -1, true);
	add_element("DarterR", "Tools", 255, 140, 0, 1, SOLID, -1, true);
	add_element("BoxMaker", "Tools", 128, 128, 128, 1, SOLID, -1, true);
	add_element("BoxMakerU", "Tools", 128, 128, 128, 1, SOLID, -1, true);
	add_element("BoxMakerD", "Tools", 128, 128, 128, 1, SOLID, -1, true);
	add_element("BoxMakerL", "Tools", 128, 128, 128, 1, SOLID, -1, true);
	add_element("BoxMakerR", "Tools", 128, 128, 128, 1, SOLID, -1, true);
	add_element("Expanding-Foam", "Tools", 255, 255, 255, 100, SOLID, -1, true);
	add_element("Nuclear Bomb", "Explosives", 200, 200, 200, 100, SOLID, -1, true);
	add_element("C-4", "Explosives", 255, 255, 255, 100, SOLID, -1, true);
	add_element("Metal Shavings", "Powders", 160, 160, 160, 8, POWDER, -1, true);
	add_element("Red Paint", "Paints & Decorative Sand", 255, 0, 0, 100, SOLID, -1, true);
	add_element("Blue Paint", "Paints & Decorative Sand", 0, 0, 255, 100, SOLID, -1, true);
	add_element("Green Paint", "Paints & Decorative Sand", 0, 255, 0, 100, SOLID, -1, true);
	add_element("Yellow Paint", "Paints & Decorative Sand", 255, 255, 0, 100, SOLID, -1, true);
	add_element("Cyan Paint", "Paints & Decorative Sand", 0, 255, 255, 100, SOLID, -1, true);
	add_element("Magenta Paint", "Paints & Decorative Sand", 255, 0, 255, 100, SOLID, -1, true);
	add_element("Red Sand", "Paints & Decorative Sand", 255, 0, 0, 5, POWDER, -1, true);
	add_element("Blue Sand", "Paints & Decorative Sand", 0, 0, 255, 5, POWDER, -1, true);
	add_element("Green Sand", "Paints & Decorative Sand", 0, 255, 0, 5, POWDER, -1, true);
	add_element("Yellow Sand", "Paints & Decorative Sand", 255, 255, 0, 5, POWDER, -1, true);
	add_element("Cyan Sand", "Paints & Decorative Sand", 0, 255, 255, 5, POWDER, -1, true);
	add_element("Magenta Sand", "Paints & Decorative Sand", 255, 0, 255, 5, POWDER, -1, true);
	add_element("Muk", "Powders", 85, 60, 0, 4, LIQUID, -1, true);
}

void Sandbox::init_reactions()
{
	add_reaction("Plant", "Air", "Plant", "Plant", 1, NORMAL, false, DIR_UP);
	add_reaction("Plant", "Water", "Plant", "Plant", 500, NORMAL, false, DIR_ALL);
	add_reaction("Sprinkler", "Air", "Sprinkler", "Water", 500, NORMAL, false, DIR_ALL);
	add_reaction("Darter", "Fire", "Fire", "Fire", 1000, NORMAL, false, DIR_ALL);
	add_reaction("DarterU", "Fire", "Fire", "Fire", 1000, NORMAL, false, DIR_ALL);
	add_reaction("DarterL", "Fire", "Fire", "Fire", 1000, NORMAL, false, DIR_ALL);
	add_reaction("DarterR", "Fire", "Fire", "Fire", 1000, NORMAL, false, DIR_ALL);
	add_reaction("DarterD", "Fire", "Fire", "Fire", 1000, NORMAL, false, DIR_ALL);
	add_reaction("Water", "Dirt", "Air", "Soil", 1000, NORMAL, false, DIR_ALL);
	add_reaction("Water", "Soil", "Air", "Soil", 20, NORMAL, false, DIR_ALL);
	add_reaction("Torch", "Air", "Torch", "Fire", 500, NORMAL, false, DIR_ALL);
	add_reaction("Oil Well", "Air", "Oil Well", "Oil", 500, NORMAL, false, DIR_ALL);
	add_reaction("Gas", "Fire", "Fire", "Fire", 1000, EXPLOSION, false, DIR_ALL);
	add_reaction("Oil", "Fire", "Fire", "Fire", 1000, NORMAL, false, DIR_ALL);
	add_reaction("Acid", "Air", "Acid", "Air", 60, NORMAL, true, DIR_ALL);
	add_reaction("Fire", "Plant", "Fire", "Fire", 1000, NORMAL, false, DIR_ALL);
	add_reaction("Fire", "Wood", "Fire", "Fire", 60, NORMAL, false, DIR_ALL);
	add_reaction("Sponge", "Water", "Sponge", "Air", 120, NORMAL, false, DIR_ALL);
	add_reaction("Scapel", "Skin", "Air", "Wound", 1000, NORMAL, false, DIR_ALL);
	add_reaction("Wound", "Air", "Wound", "Blood", 500, NORMAL, false, DIR_ALL);
	add_reaction("Brain", "Skin", "DeadBrain", "DeadSkin", 1000, NORMAL, false, DIR_ALL);
	add_reaction("DeadSkin", "Skin", "DeadSkin", "DeadSkin", 1000, NORMAL, false, DIR_ALL);
	add_reaction("Darter", "Air", "Darter", "DarterU", 1000, NORMAL, false, DIR_UP);
	add_reaction("Darter", "Air", "Darter", "DarterL", 1000, NORMAL, false, DIR_LEFT);
	add_reaction("Darter", "Air", "Darter", "DarterR", 1000, NORMAL, false, DIR_RIGHT);
	add_reaction("Darter", "Air", "Darter", "DarterD", 1000, NORMAL, false, DIR_DOWN);
	add_reaction("DarterU", "Air", "DarterU", "DarterU", 1000, NORMAL, false, DIR_UP);
	add_reaction("DarterU", "Air", "DarterU", "Wall", 100, NORMAL, false, DIR_LEFT | DIR_RIGHT);
	add_reaction("DarterU", "Air", "DarterU", "DarterL", 20, NORMAL, false, DIR_LEFT);
	add_reaction("DarterU", "Air", "DarterU", "DarterR", 20, NORMAL, false, DIR_RIGHT);
	add_reaction("DarterL", "Air", "DarterL", "DarterL", 1000, NORMAL, false, DIR_LEFT);
	add_reaction("DarterL", "Air", "DarterL", "Wall", 100, NORMAL, false, DIR_UP | DIR_DOWN);
	add_reaction("DarterL", "Air", "DarterL", "DarterU", 20, NORMAL, false, DIR_UP);
	add_reaction("DarterL", "Air", "DarterL", "DarterD", 20, NORMAL, false, DIR_DOWN);
	add_reaction("DarterR", "Air", "DarterR", "DarterR", 1000, NORMAL, false, DIR_RIGHT);
	add_reaction("DarterR", "Air", "DarterR", "Wall", 100, NORMAL, false, DIR_UP | DIR_DOWN);
	add_reaction("DarterR", "Air", "DarterR", "DarterU", 20, NORMAL, false, DIR_UP);
	add_reaction("DarterR", "Air", "DarterR", "DarterD", 20, NORMAL, false, DIR_DOWN);
	add_reaction("DarterD", "Air", "DarterD", "DarterD", 1000, NORMAL, false, DIR_DOWN);
	add_reaction("DarterD", "Air", "DarterD", "Wall", 100, NORMAL, false, DIR_LEFT | DIR_RIGHT);
	add_reaction("DarterD", "Air", "DarterD", "DarterL", 20, NORMAL, false, DIR_LEFT);
	add_reaction("DarterD", "Air", "DarterD", "DarterR", 20, NORMAL, false, DIR_RIGHT);
	add_reaction("Muk", "Air", "Muk", "Muk", 1, NORMAL, false, DIR_ALL);
	add_reaction("Nuclear Bomb", "Fire", "Fire", "Fire", 1000, CLEAR, false, DIR_ALL);
	add_reaction("C-4", "Fire", "Fire", "Fire", 1000, SQUARE, false, DIR_ALL);
	add_reaction("Expanding-Foam", "Air", "Expanding-Foam", "Expanding-Foam", 1, NORMAL, false, DIR_ALL);

}

void Sandbox::init_selfs()
{
	addself("Fire", "Air", 100);
	addself("Scapel", "Air", 50);
}

//GUI Functions.
void Sandbox::populateGUI()
{
	for (auto i = 0; i < element_counter; i++)
	{
		if (eletable[i].visible)
			element_selector->AppendItem(eletable[i].name);
	}
	for (auto i = 0; i < group_list.size(); i++)
	{
		group_selector->AppendItem(sf::String(group_list[i].name));
	}
}

void Sandbox::refill_elementbox()
{
	element_selector->Clear();
	for (auto i = 0; i < group_list[group_selector->GetSelectedItem()].elements.size(); i++)
	{
		if (eletable[group_list[group_selector->GetSelectedItem()].elements[i]].visible)
			element_selector->AppendItem(eletable[group_list[group_selector->GetSelectedItem()].elements[i]].name);
	}
	element_selector->SelectItem(group_list[group_selector->GetSelectedItem()].previous_element);
	current_element = lookupid(element_selector->GetSelectedText());
}

void Sandbox::on_element_selection()
{
	windowclick = true;
	current_element = lookupid(element_selector->GetSelectedText());
	group_list[group_selector->GetSelectedItem()].previous_element = element_selector->GetSelectedItem();
}

void Sandbox::on_group_selection()
{
	windowclick = true;
	refill_elementbox();
}

void Sandbox::on_size_slider_change()
{
	pen_size = size_slider->GetValue();
	pen_cursor.setRadius(pen_size);
	pen_cursor.setOrigin(pen_size, pen_size);
	pen_size_label->SetText("Pen Size: " + std::to_string(pen_size));
}

void Sandbox::on_ground_toggle()
{
	if (ground)
	{
		ground = false;
	}
	else
	{
		ground = true;
	}
}

void Sandbox::on_pause_toggle()
{
	if (paused)
	{
		paused = false;
	}
	else
	{
		paused = true;
	}
}

void Sandbox::react(int x1, int y1, int x2, int y2)
{
	if (check(x2, y2))
	{
		int id1 = elegrid[x1][y1];
		int id2 = elegrid[x2][y2];
		if (id1 != id2)
		{
			if (reaction_index[id1][id2].amt > 0)
			{
				// Determine direction based on relative positions
				DirectionFlags direction = DIR_NONE;
				if (y2 < y1) direction = DIR_UP;
				else if (y2 > y1) direction = DIR_DOWN;
				else if (x2 < x1) direction = DIR_LEFT;
				else if (x2 > x1) direction = DIR_RIGHT;

				for (int i = 0; i < reaction_index[id1][id2].amt; i++)
				{
					// Check if this reaction can occur in the current direction
					if ((reaction_index[id1][id2].directions[i] & direction) &&
						(getrandom_c() <= reaction_index[id1][id2].chance[i]))
					{
						create_element(x1, y1, reaction_index[id1][id2].result_a[i]);
						create_element(x2, y2, reaction_index[id1][id2].result_b[i]);
						break;
					}
				}
			}
		}
	}
}

void Sandbox::on_clear_button_click()
{
	windowclick = true;
	clear_sandbox(0);
}

sf::Vector2i Sandbox::above(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x, pos.y - 1);
}

sf::Vector2i Sandbox::aboveleft(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x - 1, pos.y - 1);
}

sf::Vector2i Sandbox::aboveright(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x + 1, pos.y - 1);
}

sf::Vector2i Sandbox::left(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x - 1, pos.y);
}

sf::Vector2i Sandbox::right(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x + 1, pos.y);
}

sf::Vector2i Sandbox::below(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x, pos.y + 1);
}

sf::Vector2i Sandbox::belowleft(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x - 1, pos.y + 1);
}

sf::Vector2i Sandbox::belowright(sf::Vector2i pos)
{
	return sf::Vector2i(pos.x + 1, pos.y + 1);
}

//Various movement helpers defined here.
sf::Vector2i Sandbox::decide_LR(sf::Vector2i pos)
{
	if (getrandom() <= 2)
	{
		if (moveable(pos, left(pos)))
			return left(pos);
	}
	else
	{
		if (moveable(pos, right(pos)))
			return right(pos);
	}
	return pos;
}

sf::Vector2i Sandbox::decide_DL_DR(sf::Vector2i pos)
{
	if (getrandom() == 0)
	{
		if (moveable(pos, belowleft(pos))) { return belowleft(pos); }
	}
	else
	{
		if (moveable(pos, belowright(pos))) { return belowright(pos); }
	}
	return pos;
}

//Various Utilities here.
std::string Sandbox::lower_case(std::string word)
{
	for (unsigned int i = 0; i < word.length(); ++i) {
		word[i] = tolower(word[i]);
		return word;
	}
}

void Sandbox::Draw_Line(float x1, float y1, float x2, float y2, int thickness, int eid)
{
	// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x < maxX; x++)
	{
		if (steep)
		{
			draw_element_circle(y / scale, x / scale, thickness, eid);
		}
		else
		{
			draw_element_circle(x / scale, y / scale, thickness, eid);
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}

int Sandbox::getrandom()
{
	return lr_chance.getnum();
}

int Sandbox::getrandom_c()
{
	return r_chance.getnum();
}

int Sandbox::MouseX()
{
	return sf::Mouse::getPosition(*owner).x;
}

int Sandbox::MouseY()
{
	return sf::Mouse::getPosition(*owner).y;
}

int Sandbox::SMouseX()
{
	return sf::Mouse::getPosition(*owner).x / scale;
}

int Sandbox::SMouseY()
{
	return sf::Mouse::getPosition(*owner).y / scale;
}


//Various element movements here.
sf::Vector2i Sandbox::watermove(sf::Vector2i pos)
{
	// First check: try moving directly down with higher probability (98%)
	if (moveable(pos, below(pos)))
	{
		if (getrandom_c() < 980)  // Increased from 90 to 980 (out of 1000)
			return below(pos);
	}

	// Second check: try diagonal down movements
	// Fix implementation of belowleft and belowright
	sf::Vector2i bl(pos.x - 1, pos.y + 1);  // Proper belowleft position
	sf::Vector2i br(pos.x + 1, pos.y + 1);  // Proper belowright position

	// Try diagonal down movements with high probability
	if (moveable(pos, bl) && getrandom_c() < 800)
		return bl;
	if (moveable(pos, br) && getrandom_c() < 800)
		return br;

	// As a last resort, try horizontal movement (less likely now)
	if (!moveable(pos, below(pos)))  // Only move horizontally if can't move down
	{
		return decide_LR(pos);
	}

	return pos;
}

sf::Vector2i Sandbox::powdermove(sf::Vector2i pos)
{
	if (moveable(pos, below(pos))) { return below(pos); }
	else if (moveable(pos, belowleft(pos)) && moveable(pos, belowright(pos)))
	{
		if (getrandom() == 0)
			return belowright(pos);
		else
			return belowleft(pos);
	}
	else if (getrandom_c() < 80 && moveable(pos, belowright(pos))) { return belowright(pos); }
	else if (getrandom_c() < 90 && moveable(pos, belowleft(pos))) { return belowleft(pos); }
	return pos;
}

sf::Vector2i Sandbox::stonemove(sf::Vector2i pos)
{
	sf::Vector2i temp_pos = decide_DL_DR(pos);
	if (temp_pos.x == pos.x)
	{
		if (moveable(pos, below(pos))) { return below(pos); }
		else if (moveable(pos, belowleft(pos)) && moveable(pos, belowright(pos)))
		{
			if (getrandom() == 0)
				return belowright(pos);
			else
				return belowleft(pos);
		}
		else if (getrandom_c() < 80 && moveable(pos, belowright(pos))) { return belowright(pos); }
		else if (getrandom_c() < 90 && moveable(pos, belowleft(pos))) { return belowleft(pos); }
		return pos;
	}
	else
	{
		return temp_pos;
	}
}

sf::Vector2i Sandbox::firemove(sf::Vector2i pos)
{
	int dir = getrandom_c();
	// Equal 25% chance for each upward direction
	if (dir < 250 && moveable(pos, above(pos)))
	{
		return above(pos);
	}
	else if (dir < 500 && moveable(pos, aboveleft(pos)))
	{
		return aboveleft(pos);
	}
	else if (dir < 750 && moveable(pos, aboveright(pos)))
	{
		return aboveright(pos);
	}
	else if (dir < 850 && moveable(pos, left(pos)))
	{
		// Small chance to move left
		return left(pos);
	}
	else if (dir < 950 && moveable(pos, right(pos)))
	{
		// Small chance to move right
		return right(pos);
	}
	return pos;
}

sf::Vector2i Sandbox::gasmove(sf::Vector2i pos)
{
	double dir = getrandom();

	int md = gas_chance.getnum();

	switch (md)
	{
	case 1:
		if (moveable(pos, above(pos)))
		{
			return above(pos);
		}
	case 2:
		if (moveable(pos, left(pos)))
		{
			return left(pos);
		}
	case 3:
		if (moveable(pos, right(pos)))
		{
			return right(pos);
		}
	case 4:
		if (moveable(pos, below(pos)))
		{
			return below(pos);
		}
	default:
		return pos;
		break;
	}
}