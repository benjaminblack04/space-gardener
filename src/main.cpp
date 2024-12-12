#include <SFML/Graphics.hpp>
#include <iostream>

#include "game.h"
#include "sceneManager.h"
#include "LevelSystem.h"
#include "misc.h"

bool leftMouseButtonPressed = false;
bool tabbedIn = true;

void Reset() {

}

void load(RenderWindow& window) {
    clog << "[load] Loading scenes..." << endl;
    initScenes();

    clog << "[load] Loading active scene..." << endl;
    activeScene->load();

    clog << "[load] Loading level..." << endl;
    ls::loadLevelFile("res/levels/arena.xml");

    clog << "[load] Setting scene windows..." << endl;
    menuScene->setWindow(window);

    mainSettingsScene->setWindow(window);
    videoSettingsScene->setWindow(window);
    audioSettingsScene->setWindow(window);
    inputSettingsScene->setWindow(window);

    gameScene->setWindow(window);

    clog << "[load] Calling reset..." << endl;
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
                clog << "[Update] Window gained focus" << endl;
                tabbedIn = true;
                break;
            }
            case Event::LostFocus: {
                clog << "[Update] Window lost focus" << endl;
                tabbedIn = false;
                break;
            }

            default:
                break;
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape) && tabbedIn) {
        clog << "[Update] Escape key pressed" << endl;
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

    clog << "Loading game..." << endl;
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
