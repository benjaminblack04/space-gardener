#pragma once

#include <SFML/Graphics.hpp>
#include <tinyxml2.h>
#include <memory>

#define ls LevelSystem

using namespace std;
using namespace sf;
using namespace tinyxml2;

class LevelSystem {
public:
    LevelSystem() = delete;
    ~LevelSystem() = delete;

    static void loadLevelFile(const char* path);
    static void Render(RenderWindow &window);

    static RectangleShape getObject(int index);

protected:
    // List of 2d tiles
    static vector<vector<unique_ptr<Sprite>>> _tiles;

    // list of objects like walls
    static vector<RectangleShape> _objects;
};
