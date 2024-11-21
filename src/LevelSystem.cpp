#include "LevelSystem.h"
#include <SFML/Graphics/Color.hpp>
#include <fstream>
#include <iostream>
#include <tinyxml2.h>

using namespace std;
using namespace sf;
using namespace tinyxml2;

sf::Texture LevelSystem::_tileset;

std::vector<std::unique_ptr<sf::Sprite>> LevelSystem::_tiles;

sf::Vector2f LevelSystem::getIsometric(int x, int y, int map_width) {
    int mapOffsetX = (map_width/2);
    int mapOffsetY = 0;

    float X = mapOffsetX * 31 + (y - x) * (31 / 2);
    float Y = mapOffsetY * 14 + (x + y) * (14 / 2);

    return {X, Y};
}

void LevelSystem::loadLevelFile(const char* path) {
    string buffer;

    _tileset.loadFromFile("res/tileset.png");

    ifstream f(path);
    if (f.good()) {
        f.seekg(0, std::ios::end);
        buffer.resize(f.tellg());
        f.seekg(0);
        f.read(&buffer[0], buffer.size());
        f.close();
    } else {
        throw string("Couldn't open level file: ") + path;
    }

    XMLDocument doc;
    doc.LoadFile(path);

    const char *data = doc.FirstChildElement("map")
                           ->FirstChildElement("layer")
                           ->FirstChildElement("data")
                           ->GetText();

    int x = 0;
    int y = 0;

    std::vector<Sprite> temp_tiles;
    for (int i = 0; i < 50; ++i) {

        if (x % 5 == 0) {
            x = 0;
            y++;
        }

        sf::IntRect subRect;
        subRect.left = 1*32;
        subRect.top = 0*17;
        subRect.width = 32;
        subRect.height = 17;

        auto s = make_unique<sf::Sprite>(_tileset, subRect);
        s->setPosition(getIsometric(x, y, 5));
        _tiles.push_back(move(s));
        x++;
    }
}

void LevelSystem::Render(RenderWindow &window) {
    for (size_t i = 0; i < _tiles.size(); ++i) {
        window.draw(*_tiles[i]);
    }
}
