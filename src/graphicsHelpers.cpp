#include "graphicsHelpers.h"

using namespace sf;

// Set text properties
void setMenuText(Text& text_field, const Font& font, unsigned int size, const Color& color, const std::string& text, const float gameWidth, const float gameHeight, const float bottomOffset) {
    text_field.setFont(font);
    text_field.setCharacterSize(size);
    text_field.setFillColor(color);
    text_field.setString(text);
    text_field.setOrigin(text_field.getLocalBounds().width / 2, text_field.getLocalBounds().height);
    text_field.setPosition(gameWidth * 0.5f, gameHeight - bottomOffset);
}

void setMenuButton(RectangleShape &button_field, Text &text_field, const Color &button_color, const Color &text_color, const Font& font, unsigned int size, const std::string& text, const float gameWidth, const float gameHeight, float bottomOffset) {
    button_field.setSize(Vector2f(menu_button_width, menu_button_height));
    button_field.setOrigin(menu_button_width * .5f, menu_button_height * .5f);
    button_field.setFillColor(button_color);
    button_field.setPosition(gameWidth * 0.5f, gameHeight - bottomOffset);

    // Set text properties
    setMenuText(text_field, font, size, text_color, text, gameWidth, gameHeight, bottomOffset);
}
