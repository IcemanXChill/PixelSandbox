#include "ElementLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

ElementLoader::ElementLoader()
{
}

ElementLoader::~ElementLoader()
{
}

std::vector<Element> ElementLoader::get_element_list()
{
    return element_list;
}

void ElementLoader::load_from_file(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    // Map to store element names to their index for reaction handling
    std::map<std::string, int> element_name_to_index;

    std::string line;
    while (std::getline(file, line))
    {
        std::string token;
        std::vector<std::string> tokens;

        // Split the line by tabs
        std::istringstream lineStream(line);
        while (std::getline(lineStream, token, '\t'))
        {
            tokens.push_back(token);
        }

        // Skip empty lines
        if (tokens.empty())
            continue;

        // Process element entries
        if (tokens[0] == "element" && tokens.size() >= 10)
        {
            std::string name = tokens[1];
            std::string group = tokens[2];

            int r, g, b;
            try {
                r = std::stoi(tokens[3]);
                g = std::stoi(tokens[4]);
                b = std::stoi(tokens[5]);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing RGB values: " << e.what() << std::endl;
                continue;
            }

            int density = 0;
            try {
                density = std::stoi(tokens[6]);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing density: " << e.what() << std::endl;
            }

            // Convert element type string to enum value
            int type_value = 0; // Default to EMPTY
            std::string element_type = tokens[7];
            if (element_type == "EMPTY") type_value = 0;
            else if (element_type == "SOLID") type_value = 1;
            else if (element_type == "LIQUID") type_value = 2;
            else if (element_type == "POWDER") type_value = 3;
            else if (element_type == "HEAT") type_value = 4;
            else if (element_type == "GAS") type_value = 5;
            else if (element_type == "STONE") type_value = 6;

            bool visible = (tokens[8] == "TRUE");

            double life = -1;
            try {
                life = std::stod(tokens[9]);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing life: " << e.what() << std::endl;
            }

            // Add element and store its index
            add_element(name, group, r, g, b, density, type_value, life, visible);

            // Store the element name to its index for later reaction processing
            element_name_to_index[name] = element_list.size() - 1;
        }
        // Process reaction entries
        else if (tokens[0] == "reaction" && tokens.size() >= 10)
        {
            std::string element1 = tokens[1];
            std::string element2 = tokens[2];
            std::string result1 = tokens[3];
            std::string result2 = tokens[4];

            double chance = 1.0;
            try {
                chance = std::stod(tokens[5]) / 1000.0; // Convert from per 1000 to probability
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing reaction chance: " << e.what() << std::endl;
            }

            int channel = 0;
            try {
                channel = std::stoi(tokens[6]);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing reaction channel: " << e.what() << std::endl;
            }

            // Parse reaction type
            std::string type_str = tokens[7];
            ReactionType reaction_type = REACTION_NORMAL;
            if (type_str == "NORMAL") reaction_type = REACTION_NORMAL;
            else if (type_str == "EXPLOSION") reaction_type = REACTION_EXPLOSION;
            else if (type_str == "PIPE") reaction_type = REACTION_PIPE;
            else if (type_str == "TELEPORT") reaction_type = REACTION_TELEPORT;

            bool all_elements = (tokens[8] == "TRUE");

            int modifier = 0;
            try {
                if (tokens.size() > 9) {
                    modifier = std::stoi(tokens[9]);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing reaction modifier: " << e.what() << std::endl;
            }

            // Parse directions
            DirectionFlags directions = DIRECTION_NONE;
            if (tokens.size() > 10) {
                std::string direction_str = tokens[10];
                if (direction_str.find('U') != std::string::npos) directions |= DIRECTION_UP;
                if (direction_str.find('D') != std::string::npos) directions |= DIRECTION_DOWN;
                if (direction_str.find('L') != std::string::npos) directions |= DIRECTION_LEFT;
                if (direction_str.find('R') != std::string::npos) directions |= DIRECTION_RIGHT;
            }

            add_reaction(element1, element2, result1, result2, chance, reaction_type, all_elements, modifier, directions);
        }
        // Process self entries
        else if (tokens[0] == "self" && tokens.size() >= 4)
        {
            std::string element1 = tokens[1];  // Element to add self to
            std::string element2 = tokens[2];  // The element it turns into

            int chance = 0;
            try {
                chance = std::stoi(tokens[3]);  // Chance out of 1000
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing self chance: " << e.what() << std::endl;
            }

            int channel = -1;  // Default: ignore channel
            if (tokens.size() >= 5) {
                try {
                    channel = std::stoi(tokens[4]);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error parsing self channel: " << e.what() << std::endl;
                }
            }

            int time = -1;  // Default: ignore time
            if (tokens.size() >= 6) {
                try {
                    time = std::stoi(tokens[5]);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error parsing self time: " << e.what() << std::endl;
                }
            }

            add_self(element1, element2, chance, channel, time);
        }
        // Process channel entries
        else if (tokens[0] == "channel" && tokens.size() >= 3)
        {
            std::string element = tokens[1];  // Element to add the channel to

            int channel_number = 0;
            try {
                channel_number = std::stoi(tokens[2]);  // The channel that it will activate
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing channel number: " << e.what() << std::endl;
            }

            add_channel(element, channel_number);
        }
    }

    file.close();
}

void ElementLoader::load_default_file()
{
    load_from_file("Default.txt");
}

void ElementLoader::add_element(std::string name, std::string group, int r, int g, int b, int density, int type, double life, bool visible)
{
    Element element;
    element.name = name;
    element.r = r;
    element.g = g;
    element.b = b;
    element.density = density;
    element.type = type;
    element.life = life;
    element.visible = visible;
    element.id = element_list.size(); // Assign ID based on position in vector

    element_list.push_back(element);
    element_groups[group].push_back(element_list.size() - 1);
}

void ElementLoader::add_reaction(std::string element1, std::string element2, std::string result1, std::string result2,
    double chance, ReactionType type, bool all_elements, int modifier, DirectionFlags directions)
{
    Reaction reaction;
    reaction.element1 = element1;
    reaction.element2 = element2;
    reaction.result1 = result1;
    reaction.result2 = result2;
    reaction.chance = chance;
    reaction.type = type;
    reaction.all_elements = all_elements;
    reaction.modifier = modifier;
    reaction.directions = directions;

    reaction_list.push_back(reaction);

    // Mark elements as reactive
    for (auto& element : element_list) {
        if (element.name == element1 || element.name == element2) {
            element.reacts = true;
            element.reactions_num++;
        }
    }
}

void ElementLoader::add_self(std::string element1, std::string element2, int chance, int channel, int time)
{
    Self self_interaction;
    self_interaction.element1 = element1;   // Element to add self to
    self_interaction.element2 = element2;   // Element it turns into
    self_interaction.chance = chance;       // Chance out of 1000
    self_interaction.channel = channel;     // Channel required (-1 to ignore)
    self_interaction.time = time;           // Time required (-1 to ignore)

    self_list.push_back(self_interaction);

    // Update self count for the element
    for (auto& element : element_list) {
        if (element.name == element1) {
            element.self_num++;
        }
    }
}

void ElementLoader::add_channel(std::string element, int channel_number)
{
    Channel channel;
    channel.element = element;           // Element to add the channel to
    channel.channel_number = channel_number; // The channel that will activate

    channel_list.push_back(channel);

    // Note: No need to update any existing Element properties as there's no channel count
    // in the Element class. If needed, you could add this functionality later.
}