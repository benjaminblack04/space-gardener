#pragma once

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

#define menu_button_width 250.f
#define menu_button_height 100.f

void setMenuText(Text& text_field, const Font& font, unsigned int size, const Color& color, const string& text, float gameWidth, float gameHeight, float bottomOffset);
void setMenuButton(RectangleShape &button_field, Text &text_field, const Color &button_color, const Color &text_color, const Font& font, unsigned int size, const string& text, float gameWidth, float gameHeight, float bottomOffset);
