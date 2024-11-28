// #include <SFML/Graphics.hpp>

// #include "LevelSystem.h"

// using namespace sf;
// using namespace std;

// CircleShape shape(100.f);

// const int gameWidth = 800;
// const int gameHeight = 600;

// void Reset() {

// }

// void load() {
//     shape.setFillColor(Color::Blue);
//     Reset();
// }

// void Update(RenderWindow& window) {
//     // Reset clock, recalculate deltatime
//     static Clock clock;
//     float dt = clock.restart().asSeconds();
//     // check and consume events
//     Event event;

//     while (window.pollEvent(event)) {
//         if (event.type == Event::Closed) {
//             window.close();
//             return;
//         }
//     }

//     // Quit Via ESC Key
//     if (Keyboard::isKeyPressed(Keyboard::Escape)) {
//         window.close();
//     }
// }

// void Render(RenderWindow& window) {
//     window.draw(shape);

// }

// int main(){
//     RenderWindow window(VideoMode({gameWidth, gameHeight}), "Tile Game");

//     load();

//     ls::loadLevelFile("res/levels/Test_level.xml");

//     while (window.isOpen()){
//         window.clear();
//         Update(window);
//         Render(window);
//         ls::Render(window);
//         window.display();
//     }

//     return 0;
// }

#include <SFML/Graphics.hpp>
#include <iostream>

#include "menu.h"
#include "scene.h"

//using namespace sf;
//using namespace std;

//CircleShape shape(100.f);

//const int gameWidth = 800;
//const int gameHeight = 600;




void Reset() {

}


void load(sf::RenderWindow& window) {
    //shape.setFillColor(Color::Blue);

    initScenes();
    activeScene->load();

    menuScene->setWindow(window);
    gameScene->setWindow(window);
    /*em.list.push_back(std::make_shared<Player>());
    for (int i = 0; i < 4; ++i) {
        em.list.push_back(std::make_shared<Enemy>());
    }*/

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


    // Quit Via ESC Key
    /*if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        window.close();
    }*/

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
        }
    }

    //em.update(dt);
    activeScene->update(dt);

}

void Render(RenderWindow& window) {
    //window.draw(shape);
    //em.render(window);

    activeScene->render(window);
}


int main() {
    RenderWindow window(VideoMode({ gameWidth, gameHeight }), "Pacman Game");


    load(window);

    while (window.isOpen()) {

        window.clear();
        Update(window);
        Render(window);
        window.display();

    }

    return 0;
}
