#pragma once

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;



const int gameWidth = 800;
const int gameHeight = 600;

extern Player player;
extern vector<Entity*> entities;

extern EntityManager em;

//const Keyboard::Key controls[4] = {
//    Keyboard::Z,
//    Keyboard::Q,
//    Keyboard::S,
//    Keyboard::D
//};
