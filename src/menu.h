#ifndef PACMAN_H
#define PACMAN_H

#include "SFML/Graphics.hpp"
#include <iostream>
#include <memory>
#include "scene.h"
#include "vector"

#include "game.h"

extern std::shared_ptr<Scene> gameScene;
extern std::shared_ptr<Scene> menuScene;
extern std::shared_ptr<Scene> activeScene;

class MenuScene : public Scene {
private:
    sf::Text menu_text;  // Texte pour l'affichage du menu
    sf::Text credit_text;
    sf::Font font;  // Police pour le texte
    //bool _alreadyLoad;

    sf::RectangleShape playButton;
    sf::Text playButton_text;

    sf::RectangleShape quitButton;
    sf::Text quitButton_text;

public:
    MenuScene() = default;
    void update(double dt) override;
    void render(sf::RenderWindow& window) override;
    void load() override;
};

class GameScene : public Scene {
private:
    sf::Text respawnText;
    sf::Font font;
    sf::Clock scoreClock;
    //bool _alreadyLoad;
    void respawn();

public:
    GameScene() = default;
    void update(double dt) override;
    void render(sf::RenderWindow& window) override;
    void load() override;
};

void initScenes();

#endif // PACMAN_H
