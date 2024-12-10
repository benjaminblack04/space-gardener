#pragma once
#include <SFML/Graphics.hpp>
#include "vector"
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>
#include <memory>
#include <tuple>
#include "math.h"
#include "utilites.h"
#include <SFML/Audio.hpp>

# define M_PI           3.14159265358979323846

class Entity {
public:
    Entity();
    virtual ~Entity();
    virtual void update(const float& dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    const sf::Vector2f& getPosition() const;
    void setPosition(const sf::Vector2f& _position);
    bool is_fordeletion() const;
    float getRotation() const;
    void setRotation(float _rotation);
    bool isAlive() const;
    void setAlive(bool _alive);
    void setForDelete();
    bool isVisible() const;
    void setVisible(bool _visible);

    float getHealth() const;
    void decreaseHealth(float damage);
    void setHealth(float health);
    float getMaxHealth() const;

    void syncShapePosition();

    virtual float getRadius() const;
    virtual const sf::Shape* getShape() const;

    bool _isMarkedForDeletion = false; // Indique si l'entité doit être supprimée
    float _timeSinceDeath = 0.0f;

protected:
    //std::vector<std::shared_ptr<Component>> _components;
    sf::Vector2f _position;
    float _rotation;
    bool _alive;       // should be updated
    bool _visible;     // should be rendered
    bool _fordeletion; // should be deleted
    float _health;
    float _maxHealth = 100;

    bool isDamaged = false;            // Indique si l'entité a récemment pris des dégâts
    float damageEffectDuration = 0.5f; // Durée pendant laquelle l'entité reste orange
    float damageEffectElapsed = 0.0f;  // Temps écoulé depuis le début de l'effet visuel

    sf::CircleShape shape;
};

class Arrow{
public:
    Arrow(const sf::Vector2f& position, const sf::Vector2f& direction);
    void update(float dt);
    void render(sf::RenderWindow& window);
    bool isExpired() const;
    void markAsExpired();

    const sf::Vector2f& getPosition() const;
    float getRadius() const;
    const sf::CircleShape& getShape() const;

private:
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::CircleShape shape;
    float speed = 500.0f;
    float lifetime = 5.0f;
    float timeElapsed = 0.0f;
};

class Player : public Entity {
public:
    Player();
    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;

    void handleMouseInput(const sf::RenderWindow& window);
    void fireArrow();

    const std::vector<Arrow>& getArrows() const;

    void drawHealthBar(sf::RenderWindow& window, sf::Font font);
    void drawSaminaBar(sf::RenderWindow& window, sf::Font font);

    void decreaseStamina(float value);
    void increaseStamina(float value);

    void dodge(const float& dt);

    //void setPosition(float x, float y) override;
    //sf::Vector2f getPosition() const override;

    bool isInvulnerable = false;   // Indique si le joueur est invulnérable
    float invulnerabilityDuration = 0.5f; // Durée de l'invulnérabilité (en secondes)
    float invulnerabilityTimeElapsed = 0.0f; // Temps écoulé depuis le début de l'invulnérabilité

    float regenTimeElapsed = 0.0f;     // Temps écoulé depuis la dernière prise de dégâts

    float getRadius() const override;
    const sf::Shape* getShape() const override;

    void addWall(sf::RectangleShape wall);

    bool isIntersectingWithWall(sf::Vector2f playerPos);
protected:
    float stamina;
    float maxStamina;

    float dodgeCost = 45.0f;    // Coût de l'esquive en stamina
    float dodgeCooldown = 1.0f; // Temps d'attente entre deux esquives (en secondes)
    float lastDodgeTime = -dodgeCooldown; // Temps écoulé depuis la dernière esquive
    float dodgeDistance = 70.0f; // Distance parcourue pendant l'esquive

    float regenDelay = 5.0f;           // Temps en secondes avant la régénération
    float regenRate = 5.0f;

    float minChargeTime = 1.5f;

    bool isCharging = false;
    sf::Vector2f aimDirection;
    sf::Vector2f aimStartPosition;
    float chargeTime = 0.0f;
    std::vector<Arrow> arrows;

    sf::SoundBuffer snd_arrow_shoot;
    sf::Sound plr_snd;

    std::vector<sf::RectangleShape> walls;
};

class AttackShortEnemy {
public:
    AttackShortEnemy(const sf::Vector2f& position, const sf::Vector2f& direction);
    void update(const float& dt);
    void render(sf::RenderWindow& window);

    bool isExpired() const;

    const sf::ConvexShape& getShape() const;

    bool hasDealtDamage = false;

private:
    sf::ConvexShape shape; // Triangle rouge
    sf::Vector2f direction; // Direction de déplacement
    float speed = 400.0f; // Vitesse
    float lifetime = 3.0f; // Durée de vie
    float timeElapsed = 0.0f;
};

class SwordAttack {
public:
    SwordAttack(const sf::Vector2f& position, const sf::Vector2f& direction);

    void update(float dt);
    void render(sf::RenderWindow& window);
    bool isExpired() const;

    const sf::ConvexShape& getShape() const;

    bool hasDealtDamage = false; // Évite que l'attaque inflige des dégâts plusieurs fois

private:
    sf::ConvexShape shape;
    sf::Vector2f direction;
    float lifetime = 0.5f;
    float timeElapsed = 0.0f;
};




class Enemy : public Entity {
public:
    Enemy();
    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;

    //void setPosition(float x, float y) override;
    //sf::Vector2f getPosition() const override;
    void getPlayer(std::shared_ptr<Player> player);

    void drawHealthBar(sf::RenderWindow& window);

    std::vector<AttackShortEnemy>& getShortAttacks();

    void triggerSwordAttack(); // Déclencher une attaque triangulaire
    std::vector<SwordAttack>& getSwordAttacks();
    void clearSwordAttacks(); // Nettoyer les attaques triangulaires

    void clearAttacks();

    float getRadius() const override;
    const sf::Shape* getShape() const override;

    bool getAlreadyCounted() const;
    void setAlreadyCounted(bool counted);


private:
    bool alreadyCounted = false;


    std::shared_ptr<Player> _player;

    std::vector<AttackShortEnemy> shortAttacks;
    float shortAttackCooldown = .5f; // Temps entre deux attaques
    float timeSinceLastShortAttack = 0.0f;

    std::vector<SwordAttack> swordAttacks; // Liste des attaques triangulaires
    float swordAttackCooldown = 1.0f; // Temps entre deux attaques triangulaires
    float timeSinceLastSwordAttack = 0.0f; // Temps écoulé depuis la dernière attaque

    sf::SoundBuffer snd_swing_sword;
    sf::Sound eny_snd;
};


struct EntityManager {
    std::vector<std::shared_ptr<Entity>> list;
    void update(double dt);
    void render(sf::RenderWindow& window);

    int score = 0; // Nouveau membre pour le score
    void increaseScore(int points); // Nouvelle fonction pour augmenter le score
    void resetScore(); // Nouvelle fonction pour réinitialiser le score
};
