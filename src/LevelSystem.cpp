#include "LevelSystem.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <tinyxml2.h>

using namespace std;
using namespace sf;
using namespace tinyxml2;

std::vector<std::vector<std::unique_ptr<sf::Sprite>>> LevelSystem::_tiles;

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

sf::Vector2f LevelSystem::getIsometric(int x, int y, int map_width) {
  int mapOffsetX = (map_width/2);
  int mapOffsetY = 0;

  float X = mapOffsetX + (x - y) * (32 / 2);
  float Y = mapOffsetY + (y + x) * (17 / 2);

  return {X, Y};
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
      s->setPosition(getIsometric(x, y, map_width));
      temp_tiles.push_back(move(s));
      x++;
    }

    _tiles.push_back(move(temp_tiles));

    layer = layer->NextSiblingElement("layer");
  }
}

void LevelSystem::Render(RenderWindow &window) {
  for (size_t i = 0; i < _tiles.size(); ++i) {
    for (size_t j = 0; j < _tiles[i].size(); ++j) {
      window.draw(*_tiles[i][j]);
    }
  }
}
