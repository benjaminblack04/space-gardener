#include <SFML/Graphics.hpp>
#include <iostream>

#include "sceneManager.h"
#include "scene.h"
#include "LevelSystem.h"
#include "misc.h"

void Reset() {

}

void load(RenderWindow& window) {
    std::clog << "[load] Loading scenes..." << std::endl;
    initScenes();

    std::clog << "[load] Loading active scene..." << std::endl;
    activeScene->load();

    std::clog << "[load] Loading level..." << std::endl;
    ls::loadLevelFile("res/levels/arena.xml");

    std::clog << "[load] Setting scene windows..." << std::endl;
    menuScene->setWindow(window);
    gameScene->setWindow(window);

    std::clog << "[load] Calling reset..." << std::endl;
    Reset();
}

void Update(RenderWindow& window) {
    // Reset clock, recalculate delta time
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
        std::clog << "[Update] Enter key pressed" << std::endl;
        if (activeScene == menuScene) {
            activeScene = gameScene;
            activeScene->load();
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        std::clog << "[Update] Enter key pressed" << std::endl;
        if (activeScene == gameScene) {
            activeScene = menuScene;
            activeScene->load();
        }
    }

    std::clog << "[Update] calling scene update" << std::endl;
    activeScene->update(dt);
}

void Render(RenderWindow& window) {
    std::clog << "[Render] calling scene render" << std::endl;
    activeScene->render(window);
    std::clog << "[Render] called scene render" << std::endl;
}

int main() {
    RenderWindow window(VideoMode({ gameWidth, gameHeight }), "Space Gardener");

    window.setFramerateLimit(fps_limit);

    std::clog << "Loading game..." << std::endl;
    load(window);

    while (window.isOpen()) {
        std::clog << "Game frame is running..." << std::endl;

        // Clear window
        window.clear();

        // Draw new frame
        std::clog << "Game frame is ready for update..." << std::endl;
        Update(window);

        std::clog << "Game frame is ready for render..." << std::endl;

        Render(window);

        std::clog << "Game frame is ready for display..." << std::endl;

        // Display window
        window.display();

        std::clog << "Game frame is finished..." << std::endl;

    }

    return 0;
}
