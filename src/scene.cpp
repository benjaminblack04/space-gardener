#include "scene.h"



void Scene::update(double dt) {
    _ents.update(dt);
}

void Scene::render(sf::RenderWindow& window) {
    _ents.render(window);
}

std::vector<std::shared_ptr<Entity>>& Scene::getEnts() {
    return _ents.list;
}

std::shared_ptr<Player> Scene::getPlayer() const {
    for (const auto& entity : _ents.list) {
        if (auto player = std::dynamic_pointer_cast<Player>(entity)) {
            return player;
        }
    }
    return nullptr;
}

void Scene::setWindow(sf::RenderWindow& window) {
    this->window = &window;
}