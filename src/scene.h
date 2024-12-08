#ifndef SCENE_H
#define SCENE_H

#include "entity.h"
#include <vector>
#include <memory>

class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual void update(double dt);
    virtual void render(sf::RenderWindow& window);
    virtual void load() = 0; // Fonction virtuelle pure pour forcer l'implémentation dans les classes dérivées

    std::vector<std::shared_ptr<Entity>>& getEnts();
    std::shared_ptr<Player> getPlayer() const;

    void setWindow(sf::RenderWindow& window);

protected:
    EntityManager _ents;
    bool _alreadyLoad = false;
    sf::RenderWindow* window = nullptr;
    sf::Font font;
};



#endif // SCENE_H
