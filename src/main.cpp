#include <SFML/Graphics.hpp>
#include <iostream>

#include "menu.h"
#include "scene.h"
#include "LevelSystem.h"

void Reset() {

}

void load(sf::RenderWindow& window) {
    initScenes();
    activeScene->load();

    ls::loadLevelFile("res/levels/arena.xml");

    menuScene->setWindow(window);
    gameScene->setWindow(window);

    Reset();
}

void Update(RenderWindow& window) {
    // Reset clock, recalculate deltatime
    static Clock clock;
    float dt = clock.restart().asSeconds();
    // check and consume events
    Event event;

    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
            return;
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Enter)) {
        if (activeScene == menuScene) {
            activeScene = gameScene;
            activeScene->load();
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        if (activeScene == gameScene) {
            activeScene = menuScene;
            activeScene->load();
        } else {
          window.close();
        }
    }

    activeScene->update(dt);
}

void Render(RenderWindow& window) {
    activeScene->render(window);
}

int main() {
    RenderWindow window(VideoMode({ gameWidth, gameHeight }), "Space Gardener");

    load(window);

    while (window.isOpen()) {
        window.clear();
        Update(window);
        Render(window);
        window.display();
    }

    return 0;
}
