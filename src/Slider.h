#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Slider
{
    sf::RectangleShape slider;
    sf::RectangleShape axis;
    sf::Font font;
    sf::Text text;

    int axisWidth;
    int axisHeight;
    int sliderWidth;
    int sliderHeight;

    float sliderValue{};
    int minValue{};
    int maxValue{};
    int xCord{};
    int yCord{};
public:
    Slider();
    sf::Text returnText(int x, int y, std::string z, int fontSize);
    void configure(int x, int y);
    void create(int min, int max);
    void logic(sf::RenderWindow &window);
    float getSliderValue();
    void setSliderValue(float newValue);
    void draw(sf::RenderWindow & window);
};