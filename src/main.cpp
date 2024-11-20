#include <SFML/Graphics.hpp>
#include <tinyxml2.h>

using namespace sf;
using namespace std;
using namespace tinyxml2;

CircleShape shape(100.f);

const int gameWidth = 800;
const int gameHeight = 600;

void Reset() {

}

void load() {
    shape.setFillColor(Color::Blue);
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
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        window.close();
    }
}

void Render(RenderWindow& window) {
    window.draw(shape);

}

int main(){
    RenderWindow window(VideoMode({gameWidth, gameHeight}), "Tile Game");

    {
        XMLDocument doc;

        doc.LoadFile("res/levels/Test_level.xml");

        const char* title = doc.FirstChildElement("map")->FirstChildElement("layer")->FirstChildElement("data")->GetText();
        printf( "Name of play (1): %s\n", title );
    }

    load();

    while (window.isOpen()){
        window.clear();
        Update(window);
        Render(window);
        window.display();
    }

    return 0;
}
