#include <SFML/Graphics.hpp>
#include <iostream>

#include "sceneManager.h"
#include "scene.h"
#include "LevelSystem.h"
#include "misc.h"

void Reset() {

}

void load(RenderWindow& window) {
    std::cout << "[load] Loading scenes..." << std::endl;
    initScenes();

    std::cout << "[load] Loading active scene..." << std::endl;
    activeScene->load();

    std::cout << "[load] Loading level..." << std::endl;
    ls::loadLevelFile("res/levels/arena.xml");

    std::cout << "[load] Setting scene windows..." << std::endl;
    menuScene->setWindow(window);
    //gameScene->setWindow(window);

    std::cout << "[load] Calling reset..." << std::endl;
    Reset();
}

void Update() {
    // Reset clock, recalculate delta time
    static Clock clock;
    float dt = clock.restart().asSeconds();


    if (Keyboard::isKeyPressed(Keyboard::Enter)) {
        std::cout << "[Update] Enter key pressed" << std::endl;
        if (activeScene == menuScene) {
            std::cout << "[Update] Trying to load game" << std::endl;
            //activeScene = gameScene;
            //activeScene->load();
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        std::cout << "[Update] Enter key pressed" << std::endl;
        // if (activeScene == gameScene) {
        //     activeScene = menuScene;
        //     activeScene->load();
        // }
    }

    std::cout << "[Update] calling scene update" << std::endl;
    activeScene->update(dt);
}

void Render(RenderWindow& window) {
    std::cout << "[Render] calling scene render" << std::endl;
    activeScene->render(window);
}

int main() {
    RenderWindow window(VideoMode({ gameWidth, gameHeight }), "Space Gardener");

    //window.setFramerateLimit(fps_limit);

    std::cout << "Loading game..." << std::endl;
    load(window);

    while (window.isOpen()) {
        std::cout << "Game is running..." << std::endl;
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
