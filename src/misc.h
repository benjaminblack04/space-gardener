#pragma once

#include <SFML/Graphics.hpp>

#define sprite_scale 2

namespace misc{
    sf::Vector2f getIsometric(int x, int y, int map_width);
}
