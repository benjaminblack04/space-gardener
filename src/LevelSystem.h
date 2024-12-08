#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <tuple>

#define ls LevelSystem

class LevelSystem {
public:
    static void loadLevelFile(const char* path);
    static void Render(sf::RenderWindow &window);
    //
    static sf::RectangleShape getObject(int index);
    //Get Tile at grid coordinate
    static sf::Sprite getTile(sf::Vector2u);
    //Get Screenspace coordinate of tile
    static sf::Vector2f getTilePosition(sf::Vector2u);
    //get the tile at screenspace pos
    static sf::Sprite getTileAt(sf::Vector2f);
protected:
    static std::vector<std::vector<std::unique_ptr<sf::Sprite>>> _tiles; //Internal array of tiles
    static std::vector<sf::RectangleShape> _objects;
private:
    LevelSystem() = delete;
    ~LevelSystem() = delete;
};
