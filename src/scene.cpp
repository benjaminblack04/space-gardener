#include "scene.h"



void Scene::update(double dt) {
    _entities.update(dt);
}

void Scene::render(RenderWindow& window) {
    _entities.render(window);
}

vector<shared_ptr<Entity>>& Scene::getEntities() {
    return _entities.list;
}

shared_ptr<Player> Scene::getPlayer() const {
    for (const auto& entity : _entities.list) {
        if (auto player = dynamic_pointer_cast<Player>(entity)) {
            return player;
        }
    }
    return nullptr;
}

void Scene::setWindow(RenderWindow& window) {
    this->window = &window;
}