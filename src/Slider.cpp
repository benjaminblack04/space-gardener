#include "Slider.h"

using namespace sf;

Slider::Slider() {
    if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
    	std::cerr << "Error: Font not found!" << std::endl;
    	return;
    }

	// Bar
	axisHeight = 10;
	axisWidth = 200;

	axis.setSize(Vector2f(axisWidth, axisHeight));
	axis.setOrigin(axisWidth * 0.5f, axisHeight * 0.5f);
	axis.setFillColor(Color(63, 63, 63));

	// Slider
	sliderWidth = 20;
	sliderHeight = 30;

	slider.setSize(Vector2f(sliderWidth, sliderHeight));
	slider.setOrigin((sliderWidth + axisWidth) * 0.5f, sliderHeight * 0.5f);
	slider.setFillColor(Color(192, 192, 192));

	// Text
	text.setFont(font);
	text.setFillColor(Color::White);
}

void Slider::configure(int x, int y)
{
	xCord = x;
	yCord = y;

	axis.setPosition(x, y);
	slider.setPosition(x, y);
}

Text Slider::returnText(int x, int y, std::string z, int fontSize) {
	text.setCharacterSize(fontSize);
	text.setPosition(x, y);
	text.setString(z);
	return text;
}

void Slider::create(int min, int max) {
	sliderValue = static_cast<float>(min);
	minValue = min;
	maxValue = max;
}

void Slider::logic(RenderWindow &window) {
	if (
		// Mouse is pressed
		Mouse::isButtonPressed(Mouse::Button::Left)

		// Mouse is in the slider
		&& (
			slider.getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y)
			|| axis.getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y)
		)

		// Mouse is in the axis
		&& Mouse::getPosition(window).x >= xCord - axisWidth * 0.5f
		&& Mouse::getPosition(window).x <= xCord + axisWidth * 0.5f
	) {


		slider.setPosition(Mouse::getPosition(window).x + axisWidth * 0.5f, yCord);
		sliderValue = minValue + (slider.getPosition().x - xCord) / axisWidth * (maxValue - minValue);
	}
}

float Slider::getSliderValue() {
	return sliderValue;
}

void Slider::setSliderValue(float newValue) {
	// Check if the new value is within the bounds
	if (minValue <= newValue && newValue <= maxValue) {

		// Update the slider value
		sliderValue = newValue;

		// Calculate the position of the slider
		float maxRange = maxValue - minValue;
		float newRange = newValue - minValue;
		float sizeOfRange = axisWidth / maxRange;
		float posX = sizeOfRange * newRange;

		// Update the position of the slider
		posX += xCord;

		// Set the position of the slider
		slider.setPosition(posX, yCord);
	}
}

void Slider::draw(RenderWindow &window)
{
	logic(window);

	// Min/Initial value
	window.draw(returnText(xCord - axisWidth * 0.5f - 10, yCord + 10, std::to_string(minValue), 20));

	// Max value
	window.draw(returnText(xCord + axisWidth * 0.5f - 10, yCord + 10, std::to_string(maxValue), 20));

	// Draw the bar
	window.draw(axis);

	// Draw the slider
	window.draw(slider);

	// Draw the current value
	window.draw(returnText(slider.getPosition().x - axisWidth * 0.5f + sliderWidth, slider.getPosition().y - sliderHeight,
		std::to_string(static_cast<int>(sliderValue)), 15));
}