#pragma once

#include <SFML/Graphics.hpp>

#define sprite_scale 2
#define fps_limit 60

using namespace sf;

namespace misc {
    Vector2f getIsometric(int x, int y, int map_width);
}
