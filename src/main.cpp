#include <SFML/Graphics.hpp>
#include <iostream>

#include "sceneManager.h"
#include "scene.h"
#include "LevelSystem.h"
#include "misc.h"

bool leftMouseButtonPressed = false;
bool tabbedIn = false;

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

    mainSettingsScene->setWindow(window);
    videoSettingsScene->setWindow(window);
    audioSettingsScene->setWindow(window);
    inputSettingsScene->setWindow(window);

    gameScene->setWindow(window);

    std::clog << "[load] Calling reset..." << std::endl;
    Reset();
}

void Update(RenderWindow& window) {
    // Reset clock, recalculate delta time
    static Clock clock;
    float dt = clock.restart().asSeconds();

    // check and consume events
    Event event{};

    while (window.pollEvent(event)) {
        switch (event.type) {
            case Event::Closed: {
                window.close();
                return;
            }
            case Event::MouseButtonPressed: {
                if (event.mouseButton.button == Mouse::Left) {
                    leftMouseButtonPressed = true;
                }

                break;
            }
            case Event::MouseButtonReleased: {
                if (event.mouseButton.button == Mouse::Left) {
                    leftMouseButtonPressed = false;
                }

                break;
            }
            case Event::GainedFocus: {
                std::clog << "[Update] Window gained focus" << std::endl;
                tabbedIn = true;
                break;
            }
            case Event::LostFocus: {
                std::clog << "[Update] Window lost focus" << std::endl;
                tabbedIn = false;
                break;
            }

            default:
                break;
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape) && tabbedIn) {
        std::clog << "[Update] Escape key pressed" << std::endl;
        if (activeScene == gameScene) {
            activeScene = menuScene;
            activeScene->load();
        }
    }

    activeScene->update(dt);
}

void Render(RenderWindow& window) {
    activeScene->render(window);
}

int main() {
    RenderWindow window(VideoMode({ gameWidth, gameHeight }), "Space Gardener");

    window.setFramerateLimit(fps_limit);

    std::clog << "Loading game..." << std::endl;
    load(window);

    while (window.isOpen()) {
        // Clear window
        window.clear();

        // Draw new frame
        Update(window);
        Render(window);

        // Display window
        window.display();
    }

    return 0;
}
