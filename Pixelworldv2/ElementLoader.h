#pragma once
#include <vector>
#include <map>
#include <string>
#include "Element.h"

// Enum for reaction types
enum ReactionType {
    REACTION_NORMAL,
    REACTION_EXPLOSION,
    REACTION_PIPE,
    REACTION_TELEPORT
};

// Using bit flags for directions
typedef unsigned char DirectionFlags;
const DirectionFlags DIRECTION_NONE = 0;
const DirectionFlags DIRECTION_UP = 1;
const DirectionFlags DIRECTION_DOWN = 2;
const DirectionFlags DIRECTION_LEFT = 4;
const DirectionFlags DIRECTION_RIGHT = 8;

class ElementLoader
{
public:
    // Structure to hold reaction data
    struct Reaction {
        std::string element1;
        std::string element2;
        std::string result1;
        std::string result2;
        double chance;
        ReactionType type;
        bool all_elements;
        int modifier;
        DirectionFlags directions;
    };
    
    // Structure to hold self-interaction data
    struct Self {
        std::string element1;    // Element to add self to
        std::string element2;    // The element it turns into
        int chance;              // Chance of this happening (out of 1000)
        int channel;             // Channel required (-1 to ignore channels)
        int time;                // Time from element creation required (-1 to ignore)
    };
    
    // Structure to hold channel data
    struct Channel {
        std::string element;     // Element to add the channel to
        int channel_number;      // The channel that it will activate when in sandbox
    };

    ElementLoader();
    ~ElementLoader();

    std::vector<Element> get_element_list();
    std::vector<Reaction> get_reaction_list() { return reaction_list; }
    std::vector<Self> get_self_list() { return self_list; }
    std::vector<Channel> get_channel_list() { return channel_list; }
    std::map<std::string, std::vector<int>> get_element_groups() { return element_groups; }

    void load_from_file(std::string filename);
    void load_default_file();

private:
    std::vector<Element> element_list;
    std::vector<Reaction> reaction_list;
    std::vector<Self> self_list;
    std::vector<Channel> channel_list;
    std::map<std::string, std::vector<int>> element_groups;

    void add_element(std::string name, std::string group, int r, int g, int b, int density, int type, double life = -1, bool visible = true);
    void add_reaction(std::string element1, std::string element2, std::string result1, std::string result2, 
                     double chance, ReactionType type, bool all_elements, int modifier = 0, DirectionFlags directions = DIRECTION_NONE);
    void add_self(std::string element1, std::string element2, int chance, int channel = -1, int time = -1);
    void add_channel(std::string element, int channel_number);
};
