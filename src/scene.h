#pragma once

#include "entity.h"

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual void update(double dt);
    virtual void render(RenderWindow& window);
    virtual void load() = 0;

    vector<shared_ptr<Entity>>& getEntities();
    shared_ptr<Player> getPlayer() const;

    void setWindow(RenderWindow& window);

protected:
    EntityManager _entities;
    bool _alreadyLoaded = false;

    RenderWindow* window = nullptr;
    Font font;
};