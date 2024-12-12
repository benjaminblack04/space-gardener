#pragma once

#include "entity.h"

using namespace std;
using namespace sf;

constexpr int gameWidth = 800;
constexpr int gameHeight = 600;

extern Player player;
extern vector<Entity*> entities;

extern EntityManager em;

extern bool leftMouseButtonPressed;
extern bool tabbedIn;

//const Keyboard::Key controls[4] = {
//    Keyboard::Z,
//    Keyboard::Q,
//    Keyboard::S,
//    Keyboard::D
//};
