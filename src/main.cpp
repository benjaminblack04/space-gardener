#include <SFML/Graphics.hpp>
#include <iostream>

#include "sceneManager.h"
#include "scene.h"
#include "LevelSystem.h"
#include "misc.h"

void Reset() {

}

void load(RenderWindow& window) {
    initScenes();
    activeScene->load();

    ls::loadLevelFile("res/levels/arena.xml");

    menuScene->setWindow(window);
    gameScene->setWindow(window);

    Reset();
}

void Update() {
    // Reset clock, recalculate delta time
    static Clock clock;
    float dt = clock.restart().asSeconds();

    // Game is active
    // std::cout << "Game is active" << std::endl;

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
        }/* else {
          window.close();
        }*/
    }

    activeScene->update(dt);
}

void Render(RenderWindow& window) {
    activeScene->render(window);
}

int main() {
    RenderWindow window(VideoMode({ gameWidth, gameHeight }), "Space Gardener");

    window.setFramerateLimit(fps_limit);

    load(window);

    while (window.isOpen()) {
        // Pause while tabbed out
        // if (gameFocused) {
            window.clear();
            Update();
            Render(window);
            window.display();
        // }
    }

    return 0;
}
