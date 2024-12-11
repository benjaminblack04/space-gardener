#ifndef GRAPHICSHELPERS_H
#define GRAPHICSHELPERS_H

#include <SFML/Graphics.hpp>

#define menu_button_width 250.f
#define menu_button_height 100.f

void setMenuText(sf::Text& text_field, const sf::Font& font, unsigned int size, const sf::Color& color, const std::string& text, float gameWidth, float gameHeight, float bottomOffset);
void setMenuButton(sf::RectangleShape &button_field, sf::Text &text_field, const sf::Color &button_color, const sf::Color &text_color, const sf::Font& font, unsigned int size, const std::string& text, float gameWidth, float gameHeight, float bottomOffset);

#endif //GRAPHICSHELPERS_H
