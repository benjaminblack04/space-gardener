#include "misc.h"

// Converts 2D X and Y into an isometric Vector2f
sf::Vector2f misc::getIsometric(int x, int y, int map_width) {
  const int mapOffsetX = map_width / 2;
  const int mapOffsetY = 0;

  float X = mapOffsetX + (x - y) * (32 / 2) * sprite_scale;
  float Y = mapOffsetY + (y + x) * (17 / 2) * sprite_scale;

  return {X, Y};
}