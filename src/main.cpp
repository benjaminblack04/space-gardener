#include <SFML/Graphics.hpp>
#include <iostream>

#include "player.h"

using namespace sf;
using namespace std;

//CircleShape shape(100.f);

const int gameWidth = 800;
const int gameHeight = 600;


void Reset() {
    
}


void load() {
    //shape.setFillColor(Color::Blue);
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
   /* window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    window.draw(message);
    window.draw(score);*/
}


int main(){
  RenderWindow window(VideoMode({gameWidth, gameHeight}), "Tile Game");
  
  
  load();

  while (window.isOpen()){

      window.clear();
      Update(window);
      Render(window);
      window.display();
    
  }

  return 0;
}