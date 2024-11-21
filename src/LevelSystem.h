#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>

#define ls LevelSystem

class LevelSystem {
public:
    static void loadLevelFile(const char* path);
    static void Render(sf::RenderWindow &window);
    //Get Tile at grid coordinate
    static sf::Sprite getTile(sf::Vector2u);
    //Get Screenspace coordinate of tile
    static sf::Vector2f getTilePosition(sf::Vector2u);
    //get the tile at screenspace pos
    static sf::Sprite getTileAt(sf::Vector2f);
    static sf::Vector2f getIsometric(int x, int y, int map_width);
protected:
    static std::vector<std::unique_ptr<sf::Sprite>> _tiles; //Internal array of tiles
    static size_t _width; //how many tiles wide is level
    static size_t _height; //how many tile high is level
    static sf::Texture _tileset;
private:
    LevelSystem() = delete;
    ~LevelSystem() = delete;
};
