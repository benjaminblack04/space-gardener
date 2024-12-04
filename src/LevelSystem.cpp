#include "LevelSystem.h"
#include "misc.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <tinyxml2.h>
#include <tuple>

using namespace std;
using namespace sf;
using namespace tinyxml2;

std::vector<std::vector<std::unique_ptr<sf::Sprite>>> LevelSystem::_tiles;
std::vector<std::tuple<std::string, sf::Vector2f, sf::Vector2f>> LevelSystem::_objects;

std::vector<std::unique_ptr<sf::Texture>> tileset;

std::vector<std::string> tokenizeString(std::string str, char delim) {
    std::vector<std::string> temp;

    std::string tempStr = "";

    for (int i = 0; i < str.length(); i++) {
        if (str.at(i) == delim) {
            temp.push_back(tempStr);
            tempStr = "";
        } else {
            tempStr.push_back(str.at(i));
        }
    }

    return temp;
}

void LevelSystem::loadLevelFile(const char* path) {
    string buffer;

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

    tileset.clear();

    for (int i = 0; i < 87; i++) {
        auto t = make_unique<sf::Texture>();
        t->loadFromFile("res/tiles/" + std::to_string(i) + ".png");
        tileset.push_back(move(t));
    }

    XMLDocument doc;
    doc.LoadFile(path);

    int map_width = doc.FirstChildElement("map")->FindAttribute("width")->Int64Value();
    int tilemap_width = 8;

    XMLNode* layer = doc.FirstChildElement("map")->FirstChildElement("layer");
    XMLNode* objects = doc.FirstChildElement("map")->FirstChildElement("objectgroup");

    while (layer != NULL) {
        std::vector<std::string> map_data = tokenizeString(layer->FirstChildElement("data")->GetText(), ',');

        std::vector<std::unique_ptr<sf::Sprite>> temp_tiles;

        float x = 0;
        float y = 0;

        for (int i = 0; i < map_data.size(); ++i) {
            if (i % map_width == 0) {
                x = 0;
                y++;
            }

            int tile = std::atoi(map_data.at(i).c_str());

            if (tile == 0) {
                auto s = make_unique<sf::Sprite>();
                temp_tiles.push_back(move(s));
                x++;
                continue;
            }

            auto s = make_unique<sf::Sprite>();
            s->setTexture(*tileset.at(tile-1));
            s->setPosition(misc::getIsometric(x, y, map_width));
            s->setScale({sprite_scale, sprite_scale});
            temp_tiles.push_back(move(s));
            x++;
        }

        _tiles.push_back(move(temp_tiles));

        layer = layer->NextSiblingElement("layer");
    }

    while (objects != NULL) {
        auto object = objects->FirstChildElement("object");

        while (object != NULL) {

            std::tuple<std::string, sf::Vector2f, sf::Vector2f> o = std::make_tuple(object->Attribute("name"),
                                                                                    sf::Vector2f((-std::stoi(object->Attribute("x"))),
                                                                                                 (std::stoi(object->Attribute("y")) / 2)),
                                                                                    sf::Vector2f(std::stoi(object->Attribute("width")) * sprite_scale,
                                                                                                 std::stoi(object->Attribute("height")) * sprite_scale));
            _objects.push_back(o);

            object = object->NextSiblingElement("object");
        }

        objects = objects->NextSiblingElement("objectgroup");
    }
}

std::tuple<std::string, sf::Vector2f, sf::Vector2f> LevelSystem::getObject(std::string name, int index) {
    int temp = 0;

    for (int i = 0; i < _objects.size(); i++) {
        if (std::get<0>(_objects[i]) == name) {
            if (temp < index) {
                temp++;
                continue;
            } else {
                return _objects[i];
            }
        }
    }

    return std::make_tuple("", sf::Vector2f(), sf::Vector2f());
}

void LevelSystem::Render(RenderWindow &window) {
    for (size_t i = 0; i < _tiles.size(); ++i) {
        for (size_t j = 0; j < _tiles[i].size(); ++j) {
            window.draw(*_tiles[i][j]);
        }
    }
}
