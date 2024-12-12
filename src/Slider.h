#pragma once

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Slider
{
    RectangleShape slider;
    RectangleShape axis;
    Font font;
    Text text;

    int axisWidth;
    int axisHeight;
    int sliderWidth;
    int sliderHeight;

    float sliderValue{};
    int minValue{};
    int maxValue{};
    int xCord{};
    int yCord{};

    Text returnText(int x, int y, string z, int fontSize);
    void logic(RenderWindow &window);
public:
    Slider();
    void configure(int x, int y);
    void create(int min, int max);
    float getSliderValue();
    void setSliderValue(float newValue);

    void draw(RenderWindow & window);
};