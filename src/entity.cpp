#include "entity.h"
#include "game.h"
#include "misc.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

const int gameWidthEntity = 800;
const int gameHeightEntity = 600;

sf::Color interpolateColor(const sf::Color& start, const sf::Color& end, float factor) {
    factor = clamp(factor, 0.0f, 1.0f); // Limiter le facteur entre 0 et 1
    return sf::Color(
        static_cast<sf::Uint8>(start.r + factor * (end.r - start.r)),
        static_cast<sf::Uint8>(start.g + factor * (end.g - start.g)),
        static_cast<sf::Uint8>(start.b + factor * (end.b - start.b))
    );
}

bool isColliding(const Entity& a, const Entity& b) {
    float distance = std::sqrt(
        std::pow(a.getPosition().x - b.getPosition().x, 2) +
        std::pow(a.getPosition().y - b.getPosition().y, 2)
    );

    float collisionDistance = 25.0f * 2; // Rayon de base pour les cercles
    return distance <= collisionDistance;
}

float distanceFromCircleToSegment(const sf::Vector2f& circleCenter, const sf::Vector2f& p1, const sf::Vector2f& p2) {
    sf::Vector2f d = p2 - p1; // Vecteur du segment
    sf::Vector2f f = circleCenter - p1;

    float t = (f.x * d.x + f.y * d.y) / (d.x * d.x + d.y * d.y);
    t = std::max(0.0f, std::min(1.0f, t)); // Contraindre t à l'intervalle [0, 1]

    sf::Vector2f closestPoint = p1 + t * d; // Point le plus proche sur le segment
    float distance = std::sqrt(std::pow(closestPoint.x - circleCenter.x, 2) +
                               std::pow(closestPoint.y - circleCenter.y, 2));

    return distance;
}

bool isPointInTriangle(const sf::Vector2f& p, const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::Vector2f& p2) {
    // Calculer les vecteurs
    sf::Vector2f v0 = p2 - p0;
    sf::Vector2f v1 = p1 - p0;
    sf::Vector2f v2 = p - p0;

    // Calculer les produits scalaires
    float dot00 = v0.x * v0.x + v0.y * v0.y;
    float dot01 = v0.x * v1.x + v0.y * v1.y;
    float dot02 = v0.x * v2.x + v0.y * v2.y;
    float dot11 = v1.x * v1.x + v1.y * v1.y;
    float dot12 = v1.x * v2.x + v1.y * v2.y;

    // Calculer les barycentriques
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Vérifier si le point est dans le triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

bool isPointInShape(const sf::Shape& shape, const sf::Vector2f& point) {
    if (const sf::ConvexShape* convexShape = dynamic_cast<const sf::ConvexShape*>(&shape)) {
        if (convexShape->getPointCount() >= 3) {
            for (size_t i = 0; i < convexShape->getPointCount(); ++i) {
                sf::Vector2f p0 = convexShape->getTransform().transformPoint(convexShape->getPoint(i));
                sf::Vector2f p1 = convexShape->getTransform().transformPoint(convexShape->getPoint((i + 1) % convexShape->getPointCount()));
                sf::Vector2f p2 = convexShape->getTransform().transformPoint(convexShape->getPoint((i + 2) % convexShape->getPointCount()));

                if (isPointInTriangle(point, p0, p1, p2)) {
                    return true; // Collision si le point est dans un triangle
                }
            }
        }
    }
    return false;
}

bool isCollidingAttack(const sf::Shape& shape, const Entity& entity) {
    sf::Vector2f circleCenter = entity.getPosition();
    float circleRadius = entity.getRadius();

    // Cas 1 : Si la forme est un cercle
    if (const sf::CircleShape* otherCircle = dynamic_cast<const sf::CircleShape*>(&shape)) {
        sf::Vector2f otherCenter = otherCircle->getPosition();
        float otherRadius = otherCircle->getRadius();

        float distance = std::sqrt(
            std::pow(circleCenter.x - otherCenter.x, 2) +
            std::pow(circleCenter.y - otherCenter.y, 2)
        );

        return distance <= (circleRadius + otherRadius); // Collision si les cercles se chevauchent
    }

    // Cas 2 : Si la forme est un polygone (sf::ConvexShape)
    if (const sf::ConvexShape* convexShape = dynamic_cast<const sf::ConvexShape*>(&shape)) {
        // Vérifier si le joueur est complètement dans le polygone
        if (isPointInShape(*convexShape, circleCenter)) {
            return true;
        }

        // Vérifier si un des sommets du polygone est dans le cercle
        for (size_t i = 0; i < convexShape->getPointCount(); ++i) {
            sf::Vector2f point = convexShape->getTransform().transformPoint(convexShape->getPoint(i));
            float distance = std::sqrt(std::pow(point.x - circleCenter.x, 2) + std::pow(point.y - circleCenter.y, 2));
            if (distance <= circleRadius) {
                return true; // Collision si un sommet est dans le cercle
            }
        }

        // Vérifier la distance entre chaque segment du polygone et le cercle
        for (size_t i = 0; i < convexShape->getPointCount(); ++i) {
            sf::Vector2f p1 = convexShape->getTransform().transformPoint(convexShape->getPoint(i));
            sf::Vector2f p2 = convexShape->getTransform().transformPoint(convexShape->getPoint((i + 1) % convexShape->getPointCount()));

            if (distanceFromCircleToSegment(circleCenter, p1, p2) <= circleRadius) {
                return true; // Collision si le cercle touche un segment
            }
        }
    }

    // Cas 3 : Si la forme est un rectangle
    if (const sf::RectangleShape* rectangleShape = dynamic_cast<const sf::RectangleShape*>(&shape)) {
        // Traiter le rectangle comme un polygone avec 4 sommets
        sf::ConvexShape rectangleAsPolygon(4);
        for (size_t i = 0; i < 4; ++i) {
            rectangleAsPolygon.setPoint(i, rectangleShape->getTransform().transformPoint(rectangleShape->getPoint(i)));
        }
        return isCollidingAttack(rectangleAsPolygon, entity); // Réutiliser la logique des polygones
    }

    return false; // Pas de collision si aucune condition n'est satisfaite
}


void EntityManager::update(double dt) {
    std::clog << "EntityManager::update" << std::endl;
    // Obtenir le joueur
    Player* player = nullptr;
    for (auto& entity : list) {
        if (auto p = dynamic_cast<Player*>(entity.get())) {
            player = p;
            break;
        }
    }

    if (!player) {
        return; // Ne pas continuer si le joueur n'est pas trouvé
    }

    // Mise à jour des entités
    for (auto& entity : list) {
        if (!entity->isAlive()) continue; // Ignorer les entités mortes
        entity->update(dt);
    }

    // Vérification des collisions entre les entités
    for (size_t i = 0; i < list.size(); ++i) {
        for (size_t j = i + 1; j < list.size(); ++j) {
            // Vérifier si les deux entités sont en vie
            if (list[i]->isAlive() && list[j]->isAlive() && isColliding(*list[i], *list[j])) {
                auto playerEntity = dynamic_cast<Player*>(list[i].get());
                auto enemyEntity = dynamic_cast<Enemy*>(list[j].get());

                // Gérer les collisions joueur/ennemi
                if (playerEntity && enemyEntity && !playerEntity->isInvulnerable) {
                    playerEntity->decreaseHealth(1);
                    playerEntity->regenTimeElapsed = 0.0f;
                    enemyEntity->decreaseHealth(1);
                }
                else if (enemyEntity && playerEntity && !playerEntity->isInvulnerable) {
                    enemyEntity->decreaseHealth(1);
                    playerEntity->decreaseHealth(1);
                    playerEntity->regenTimeElapsed = 0.0f;
                }
            }
        }
    }

    // Vérification des collisions entre ennemis
    for (size_t i = 0; i < list.size(); ++i) {
        auto enemy1 = dynamic_cast<Enemy*>(list[i].get());
        if (!enemy1 || !enemy1->isAlive()) continue;

        for (size_t j = i + 1; j < list.size(); ++j) {
            auto enemy2 = dynamic_cast<Enemy*>(list[j].get());
            if (!enemy2 || !enemy2->isAlive()) continue;

            if (isColliding(*enemy1, *enemy2)) {
                // Calcul de la direction pour séparer les ennemis
                sf::Vector2f dir = enemy1->getPosition() - enemy2->getPosition();
                float magnitude = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (magnitude != 0) dir /= magnitude; // Normalisation

                // Déplacement pour éviter le chevauchement
                float overlap = 50.0f - magnitude; // Suppose un rayon de collision de 25 pour chaque ennemi
                enemy1->setPosition(enemy1->getPosition() + dir * (overlap / 2.0f));
                enemy1->syncShapePosition(); // Synchroniser la forme avec la nouvelle position

                enemy2->setPosition(enemy2->getPosition() - dir * (overlap / 2.0f));
                enemy2->syncShapePosition(); // Synchroniser la forme avec la nouvelle position
            }
        }
    }

    // Gestion des collisions entre attaques triangulaires et joueur
    if (player && player->isAlive()) {
        for (auto& entity : list) {
            if (auto enemy = std::dynamic_pointer_cast<Enemy>(entity)) {
                for (auto& attack : enemy->getSwordAttacks()) {
                    if (isCollidingAttack(attack.getShape(), *player)) {
                        player->decreaseHealth(10); // Inflige des dégâts au joueur
                        player->regenTimeElapsed = 0.0f;
                        attack.hasDealtDamage = true; // Marque l'attaque comme utilisée
                    }
                }
            }
        }
    }


    for (auto& entity : list) {
        if (auto enemy = dynamic_cast<Enemy*>(entity.get())) {
            if (!enemy->isAlive()) {
                enemy->clearAttacks();
                entity->_timeSinceDeath += dt;
                if (entity->_timeSinceDeath >= 3.0f) {
                    entity->_isMarkedForDeletion = true;
                }
                if (!enemy->getAlreadyCounted()) { // Utilisez un drapeau pour éviter les doubles comptages
                    increaseScore(1); // Ajoutez 10 points au score
                    enemy->setAlreadyCounted(true); // Marquez l'ennemi comme compté
                }
                continue;
            }

            for (auto& attack : enemy->getShortAttacks()) {
                if (!attack.hasDealtDamage && isCollidingAttack(attack.getShape(), *player) && !player->isInvulnerable) {
                    player->decreaseHealth(10); // Inflige des dégâts au joueur
                    player->regenTimeElapsed = 0.0f;
                    attack.hasDealtDamage = true; // Marque l'attaque comme utilisée
                }
            }
        }
    }


    for (const auto& entity : list) {
        auto enemy = dynamic_cast<Enemy*>(entity.get());
        if (!enemy || !enemy->isAlive()) continue;

        // Accéder aux flèches tirées par le joueur
        auto player = dynamic_cast<Player*>(list[0].get()); // Suppose que le joueur est la première entité
        if (!player) continue;

        for (const auto& arrow : player->getArrows()) {
            auto shape = arrow.getShape();
            if (isCollidingAttack(shape, *enemy)) {
                enemy->decreaseHealth(10); // Inflige des dégâts à l'ennemi
                player->regenTimeElapsed = 0.0f;
                //const_cast<Arrow&>(arrow).markAsExpired(); // Marque la flèche comme utilisée
            }
        }
    }

    list.erase(std::remove_if(list.begin(), list.end(),
        [](const std::shared_ptr<Entity>& entity) {
            return entity->_isMarkedForDeletion;
        }),
        list.end());
}



void EntityManager::render(sf::RenderWindow& window) {
    std::clog << "EntityManager::render" << std::endl;
    for (auto& entity : list) {
        std::clog << "Entity::render" << std::endl;
        entity->render(window);
    }
}

void EntityManager::increaseScore(int points) {
    score += points;
}

void EntityManager::resetScore() {
    score = 0;
}

Entity::Entity() : _position(0.0f, 0.0f), _rotation(0.0f), _alive(true), _visible(true), _fordeletion(false), _health(100) {}


Entity::~Entity() {}

const sf::Vector2f& Entity::getPosition() const {
    return _position;
}

void Entity::setPosition(const sf::Vector2f& position) {
    _position = position;
    shape.setPosition(position);
}

// Vérifier si l'entité doit être supprimée
bool Entity::is_fordeletion() const {
    return _fordeletion;
}

// Obtenir la rotation
float Entity::getRotation() const {
    return _rotation;
}

// Définir la rotation
void Entity::setRotation(float rotation) {
    _rotation = rotation;
}

// Vérifier si l'entité est vivante
bool Entity::isAlive() const {
    return _alive;
}

// Définir l'état vivant/mort de l'entité
void Entity::setAlive(bool alive) {
    _alive = alive;
}

// Marquer l'entité pour suppression
void Entity::setForDelete() {
    _fordeletion = true;
}

// Vérifier si l'entité est visible
bool Entity::isVisible() const {
    return _visible;
}

// Définir la visibilité de l'entité
void Entity::setVisible(bool visible) {
    _visible = visible;
}


float Entity::getHealth() const {
    return _health;
}

void Entity::decreaseHealth(float damage) {
    _health -= damage;
    if (_health <= 0) {
        _alive = false;
        shape.setFillColor(sf::Color(139, 69, 19));
        //std::cout << "Entity is dead!" << std::endl;
    }
    else {
        // Activer l'effet de dégâts
        isDamaged = true;
        damageEffectElapsed = 0.0f;
        shape.setFillColor(sf::Color(255, 65, 0)); // Couleur orange
    }
}

void Entity::setHealth(float health) {
    _health = health;
}

float Entity::getMaxHealth() const {
    return _maxHealth;
}

void Entity::syncShapePosition() {
    shape.setPosition(_position);
}

float Entity::getRadius() const {
    // Par défaut, retourner 0 si l'entité n'a pas de cercle
    return 0.0f;
}

const sf::Shape* Entity::getShape() const {
    // Par défaut, retourner un pointeur nul
    return nullptr;
}


Arrow::Arrow(const sf::Vector2f& position, const sf::Vector2f& direction)
    : position(position), direction(direction) {
    shape.setRadius(5.0f);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(5.0f, 5.0f);
    shape.setPosition(position);
}

void Arrow::update(float dt) {
    position += direction * speed * dt;
    shape.setPosition(position);
    timeElapsed += dt;

    //std::cout << "Arrow updated to: " << position.x << ", " << position.y << std::endl;
}

void Arrow::render(sf::RenderWindow& window) {
    window.draw(shape);
}

bool Arrow::isExpired() const {
    return timeElapsed >= lifetime;
}

void Arrow::markAsExpired() {
    timeElapsed = lifetime; // Force l'expiration de la flèche
}

const sf::Vector2f& Arrow::getPosition() const {
    return position;
}

float Arrow::getRadius() const {
    return shape.getRadius();
}

const sf::CircleShape& Arrow::getShape() const {
    return shape;
}

Player::Player() : stamina(100), maxStamina(100) {
    float player_size = 25.0f;
    shape.setRadius(player_size);
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin(player_size, player_size);
}

bool Player::isIntersectingWithWall(sf::Vector2f playerPos) {
    // NOTE: Never let me write collision code again
    sf::Vector2f playerSize = {16, 16};

    playerPos.x = playerPos.x - (playerSize.x / 2);
    playerPos.y = playerPos.y - (playerSize.y / 2);

    for (auto wall : walls) {
        auto wallPos = wall.getPosition();
        auto wallSize = wall.getSize();

        if (((playerPos.x >= wallPos.x && playerPos.x <= (wallPos.x + wallSize.x)) &&
             (playerPos.y >= wallPos.y && playerPos.y <= (wallPos.y + wallSize.y))) ||
            (((playerPos.x + playerSize.x) >= wallPos.x && (playerPos.x + playerSize.x) <= (wallPos.x + wallSize.x)) &&
             ((playerPos.y + playerSize.y) >= wallPos.y && (playerPos.y + playerSize.y) <= (wallPos.y + wallSize.y)))) {
            return true;
        }
    }

    return false;
}

void Player::update(const float& dt) {
    float moveSpeed = 200.0f;
    sf::Vector2f movement(0.0f, 0.0f);

    // Movement/make sure the player cannot move into a wall
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) &&
        !isIntersectingWithWall({shape.getPosition().x - (moveSpeed * dt), shape.getPosition().y})) { // Left
        movement.x -= moveSpeed * dt;
    } else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D)) &&
               !isIntersectingWithWall({shape.getPosition().x + (moveSpeed * dt), shape.getPosition().y})) { // Right
        movement.x += moveSpeed * dt;
    }
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) &&
        !isIntersectingWithWall({shape.getPosition().x, shape.getPosition().y - (moveSpeed * dt)})) { // Up
        movement.y -= moveSpeed * dt;
    } else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) &&
        !isIntersectingWithWall({shape.getPosition().x, shape.getPosition().y + (moveSpeed * dt)})) { // Up
        movement.y += moveSpeed * dt;
    }

    if (movement.x != 0.0f || movement.y != 0.0f) {
        // Calculer l'angle en radians
        float angle = std::atan2(movement.y, movement.x);

        // Convertir en degrés et appliquer la rotation
        setRotation(angle * 180.0f / M_PI);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        dodge(dt);
        isInvulnerable = true;
        invulnerabilityTimeElapsed = 0.0f;
    }

    if (isInvulnerable) {
        shape.setFillColor(sf::Color::Cyan);
        invulnerabilityTimeElapsed += dt;
        if (invulnerabilityTimeElapsed >= invulnerabilityDuration) {
            isInvulnerable = false; // Désactiver après la durée
            shape.setFillColor(sf::Color::Yellow);
        }
    }

    if (_alive && _health < _maxHealth) {
        regenTimeElapsed += dt; // Accumuler le temps écoulé
        if (regenTimeElapsed >= regenDelay) {
            // Ajouter des points de vie progressivement
            _health += regenRate * dt;
            if (_health > _maxHealth) {
                _health = _maxHealth; // Ne pas dépasser la vie maximale
            }
        }
    }

    increaseStamina(1);

    // Déplacer le cercle
    shape.move(movement);

    if (isDamaged) {
        damageEffectElapsed += dt;
        if (damageEffectElapsed >= damageEffectDuration) {
            isDamaged = false;
            shape.setFillColor(sf::Color::Yellow); // Remettre à la couleur normale
        }
    }

    for (auto& arrow : arrows) {
        arrow.update(dt);
    }

    // Mettre à jour la position interne
    _position = shape.getPosition();

    // Supprimer les flèches expirées
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(), [](const Arrow& arrow) {
        return arrow.isExpired();
        }), arrows.end());
}

void Player::render(sf::RenderWindow& window) {
    window.draw(shape);

    // Dessiner les flèches
    for (auto& arrow : arrows) {
        //std::cout << "Arrow position: " << arrow.getPosition().x << ", " << arrow.getPosition().y << std::endl;
        arrow.render(window);
    }

    // Dessiner l'indicateur de visée si le joueur charge une attaque
    if (isCharging) {

        sf::Color startColor = sf::Color::Blue;
        sf::Color endColor = sf::Color::Red;
        float chargeRatio = std::min(chargeTime / minChargeTime, 1.0f); // Ratio entre 0 et 1
        sf::Color indicatorColor = interpolateColor(startColor, endColor, chargeRatio);


        // Assurez-vous que l'origine du rectangle suit le joueur
        aimStartPosition = _position;

        // Calculer la longueur du rectangle (distance de visée)
        float length = 100.0f; // Longueur de la visée

        // Créer le rectangle
        sf::RectangleShape rectangle(sf::Vector2f(length, 10.0f)); // Longueur et largeur du rectangle
        rectangle.setFillColor(indicatorColor); // Couleur du rectangle
        rectangle.setOrigin(0.0f, 5.0f); // Aligner l'origine pour une rotation correcte
        rectangle.setPosition(aimStartPosition); // Définir le point de départ

        // Calculer l'angle de rotation
        float angle = std::atan2(aimDirection.y, aimDirection.x) * 180.0f / M_PI;
        rectangle.setRotation(angle); // Appliquer la rotation

        // Dessiner le rectangle
        window.draw(rectangle);
    }
}

void Player::addWall(sf::RectangleShape wall) {
    walls.push_back(wall);
}

void Player::handleMouseInput(const sf::RenderWindow& window) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (!isCharging) {
            isCharging = true;
            aimStartPosition = _position; // La position actuelle du joueur
            chargeTime = 0.0f; // Réinitialise le temps de charge
        }

        // Mise à jour de la direction de visée
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        aimDirection = worldPos - aimStartPosition;

        // Normaliser la direction
        float magnitude = std::sqrt(aimDirection.x * aimDirection.x + aimDirection.y * aimDirection.y);
        if (magnitude != 0) aimDirection /= magnitude;

        chargeTime += 0.1f; // Simuler l'accumulation de charge
    }
    else if (isCharging) {
        isCharging = false;

        // Vérifie si le joueur a chargé assez longtemps
        if (chargeTime >= minChargeTime) {
            fireArrow(); // Tire une flèche
        }

        chargeTime = 0.0f; // Réinitialise le temps de charge
    }
}

void Player::fireArrow() {
    arrows.emplace_back(_position, aimDirection);
}

const std::vector<Arrow>& Player::getArrows() const {
    return arrows;
}

void Player::drawHealthBar(sf::RenderWindow& window, sf::Font font) {
    if (getHealth() <= 0) { setHealth(0); }

    float maxHealth = getMaxHealth();
    float currentHealth = getHealth();

    float barWidth = 150.0f; // Largeur de la barre de vie
    float barHeight = 20.0f; // Hauteur de la barre
    float healthPercentage = currentHealth / maxHealth;

    // Obtenez la position du joueur
    sf::Vector2f playerPosition = getPosition();

    float position_x = playerPosition.x - 380;
    float position_y = playerPosition.y - 280;

    // Rectangle de fond (rouge)
    sf::RectangleShape healthBarBackground(sf::Vector2f(barWidth, barHeight));
    healthBarBackground.setFillColor(sf::Color::Black);
    healthBarBackground.setPosition(position_x, position_y); // En dessous du joueur

    // Rectangle pour la vie restante (vert)
    sf::RectangleShape healthBar(sf::Vector2f((barWidth * healthPercentage) - 6, (barHeight - 6)));
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition(position_x + 3, position_y + 3);

    // Dessiner les barres
    window.draw(healthBarBackground); // Fond rouge
    window.draw(healthBar);           // Vie verte
}

void Player::drawSaminaBar(sf::RenderWindow& window, sf::Font font) {
    float barWidth = 150.0f; // Largeur de la barre de vie
    float barHeight = 20.0f; // Hauteur de la barre
    float StaminaPercentage = stamina / maxStamina;

    // Obtenez la position du joueur
    sf::Vector2f playerPosition = getPosition();

    float position_x = playerPosition.x - 380;
    float position_y = playerPosition.y - 250;

    // Rectangle de fond (rouge)
    sf::RectangleShape StaminaBarBackground(sf::Vector2f(barWidth, barHeight));
    StaminaBarBackground.setFillColor(sf::Color::Black);
    StaminaBarBackground.setPosition(position_x, position_y); // En dessous du joueur

    // Rectangle pour la vie restante (vert)
    sf::RectangleShape StaminaBar(sf::Vector2f((barWidth * StaminaPercentage) - 6, barHeight - 6));
    StaminaBar.setFillColor(sf::Color::Green);
    StaminaBar.setPosition(position_x + 3, position_y + 3);

    // Dessiner les barres
    window.draw(StaminaBarBackground); // Fond rouge
    window.draw(StaminaBar);           // Vie verte
}

void Player::decreaseStamina(float value) {
    stamina -= value;
    if (stamina < 0) stamina = 0;
}

void Player::increaseStamina(float value) {
    stamina += value;
    if (stamina > maxStamina) stamina = maxStamina;
}

void Player::dodge(const float& dt) {
    static sf::Clock clock;
    float currentTime = clock.getElapsedTime().asSeconds();

    // Vérifiez si le joueur peut esquiver
    if (stamina >= dodgeCost && (currentTime - lastDodgeTime) >= dodgeCooldown) {
        // Consomme de la stamina
        decreaseStamina(dodgeCost);
        lastDodgeTime = currentTime;

        // Direction d'esquive (avant selon la rotation actuelle)
        sf::Vector2f dodgeDirection(std::cos(getRotation() * M_PI / 180.0f),
            std::sin(getRotation() * M_PI / 180.0f));
        setPosition(getPosition() + dodgeDirection * dodgeDistance);

        // Synchroniser la position avec le sprite
        syncShapePosition();
    }
}

float Player::getRadius() const {
    return shape.getRadius(); // Retourne le rayon du cercle
}


const sf::Shape* Player::getShape() const {
    return &shape; // Retourne un pointeur vers la `shape`
}

Enemy::Enemy() {
    /*_player = player; */
    float enemy_size = 25.0f;
    shape.setRadius(enemy_size);
    shape.setFillColor(sf::Color::Green);
    shape.setOrigin(enemy_size, enemy_size);
}

void Enemy::update(const float& dt) {
    if (!_alive) {
        shape.setFillColor(sf::Color(139, 69, 19));
        shortAttacks.clear(); // Supprimer toutes les attaques si l'ennemi est mort
        clearSwordAttacks();
        return;
    }

    if (_player && _player->isAlive() == false) return;
    /*float moveSpeed = 100.0f;
    float dx = (rand() % 3 - 1) * moveSpeed * dt;
    float dy = (rand() % 3 - 1) * moveSpeed * dt;
    shape.move(dx, dy);*/



    //// Obtenir la position du joueur
    const sf::Vector2f& playerPosition = _player->getPosition();

    float dx = playerPosition.x - _position.x;
    float dy = playerPosition.y - _position.y;
    float distance_player = std::sqrt(dx * dx + dy * dy);

    //// Définir une distance seuil
    float threshold_move = 350.0f;
    float threshold_short_attack = 200.0f;

    float moveSpeed = .7f;

    if (distance_player > threshold_move) {
        // Se rapprocher du joueur
        sf::Vector2f direction(dx, dy); // Normaliser le vecteur dx / (distance_player - 10), dy / (distance_player - 10)
        shape.setFillColor(sf::Color::Green);
        shape.move(direction * moveSpeed * dt);
    }
    else if (threshold_move >= distance_player && distance_player > threshold_short_attack) {
        sf::Vector2f direction(dx, dy); // Normaliser le vecteur dx / (distance_player - 10), dy / (distance_player - 10)
        shape.setFillColor(sf::Color::Green);
        shape.move(direction * moveSpeed * dt);

        shape.setFillColor(sf::Color::Red);
        timeSinceLastShortAttack += dt;
    }
    else if (threshold_short_attack >= distance_player) {
        shape.setFillColor(sf::Color::Magenta);
        timeSinceLastSwordAttack += dt;
    }


    if (timeSinceLastShortAttack >= shortAttackCooldown) {
        timeSinceLastShortAttack = 0.0f;

        // Calculer la direction vers le joueur
        sf::Vector2f playerPos = _player->getPosition();
        sf::Vector2f direction = sf::Vector2f(playerPos.x - _position.x, playerPos.y - _position.y);
        float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (magnitude != 0) direction /= magnitude;

        // Ajouter une attaque courte
        shortAttacks.emplace_back(_position, direction);
    }

    if (timeSinceLastSwordAttack >= swordAttackCooldown) {
        triggerSwordAttack();
    }

    // Mettre à jour les attaques courtes
    for (auto& attack : shortAttacks) {
        attack.update(dt);
    }

    for (auto& attack : swordAttacks) {
        attack.update(dt);
    }

    if (isDamaged) {
        damageEffectElapsed += dt;
        if (damageEffectElapsed >= damageEffectDuration) {
            isDamaged = false;
            shape.setFillColor(sf::Color::Green); // Remettre à la couleur normale
        }
    }

    // Supprimer les attaques expirées
    shortAttacks.erase(std::remove_if(shortAttacks.begin(), shortAttacks.end(),
        [](const AttackShortEnemy& attack) {
            return attack.isExpired() || attack.hasDealtDamage;
        }),
        shortAttacks.end());

    swordAttacks.erase(std::remove_if(swordAttacks.begin(), swordAttacks.end(),
        [](const SwordAttack& attack) { return attack.isExpired() || attack.hasDealtDamage; }),
        swordAttacks.end());



    //// Mettre à jour la position interne
    _position = shape.getPosition();
}

void Enemy::render(sf::RenderWindow& window) {
    window.draw(shape);

    // Dessiner les attaques courtes
    for (auto& attack : shortAttacks) {
        attack.render(window);
    }

    for (auto& attack : swordAttacks) {
        attack.render(window);
    }
}

void Enemy::getPlayer(std::shared_ptr<Player> player) {
    _player = player;
}

//static float calculateDistancePlayer(const sf::Vector2f& pos1, const sf::Vector2f& pos2) {
//    return sqrtf(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
//}

void Enemy::drawHealthBar(sf::RenderWindow& window) {
    if (_maxHealth <= 0) return; // Éviter les divisions par zéro

    // Calculer la largeur de la barre en fonction des points de vie actuels
    float barWeightConst = 50.0;
    float barWidth = barWeightConst * (_health / static_cast<float>(_maxHealth));
    float barHeight = 5.0f;

    // Rectangle pour la barre de vie
    sf::RectangleShape healthBarBackground(sf::Vector2f(barWeightConst, barHeight));
    healthBarBackground.setFillColor(sf::Color::Red);
    healthBarBackground.setPosition(_position.x - barWeightConst * .5f, _position.y - 45.f); // Décalage au-dessus de l'entité

    sf::RectangleShape healthBar(sf::Vector2f(barWidth, barHeight));
    healthBar.setFillColor(sf::Color::Green);
    healthBar.setPosition(_position.x - barWeightConst * .5f, _position.y - 45.f);

    // Dessiner les deux rectangles
    window.draw(healthBarBackground); // Fond rouge
    window.draw(healthBar);           // Barre verte
}

std::vector<AttackShortEnemy>& Enemy::getShortAttacks() {
    return shortAttacks;
}

void Enemy::clearAttacks() {
    shortAttacks.clear(); // Supprime toutes les attaques courtes
}

bool Enemy::getAlreadyCounted() const {
    return alreadyCounted;
}

void Enemy::setAlreadyCounted(bool counted) {
    alreadyCounted = counted;
}


void Enemy::triggerSwordAttack() {
    if (!_player) return;

    // Calculer la direction vers le joueur
    sf::Vector2f direction = _player->getPosition() - _position;
    float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (magnitude != 0) direction /= magnitude;

    // Ajouter une nouvelle attaque triangulaire
    swordAttacks.emplace_back(_position, direction);
    timeSinceLastSwordAttack = 0.0f; // Réinitialiser le cooldown
}

std::vector<SwordAttack>& Enemy::getSwordAttacks() {
    return swordAttacks;
}

void Enemy::clearSwordAttacks() {
    swordAttacks.clear();
}

float Enemy::getRadius() const {
    return shape.getRadius(); // Retourne le rayon du cercle
}

const sf::Shape* Enemy::getShape() const {
    return &shape; // Retourne un pointeur vers la `shape`
}


AttackShortEnemy::AttackShortEnemy(const sf::Vector2f& position, const sf::Vector2f& direction)
    : direction(direction) {
    shape.setPointCount(3);
    shape.setPoint(0, sf::Vector2f(0, 0));       // Sommet
    shape.setPoint(1, sf::Vector2f(-10, 20));    // Bas-gauche
    shape.setPoint(2, sf::Vector2f(10, 20));     // Bas-droit
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(position);

    // Calculer la rotation du triangle
    float angle = std::atan2(direction.y, direction.x) * 180.0f / M_PI + 90.0f;
    shape.setRotation(angle);
}

void AttackShortEnemy::update(const float& dt) {
    shape.move(direction * speed * dt);
    timeElapsed += dt;
}

void AttackShortEnemy::render(sf::RenderWindow& window) {
    window.draw(shape);
}

bool AttackShortEnemy::isExpired() const {
    return timeElapsed >= lifetime;
}

const sf::ConvexShape& AttackShortEnemy::getShape() const {
    return shape;
}

SwordAttack::SwordAttack(const sf::Vector2f& position, const sf::Vector2f& direction)
    : direction(direction) {
    // Créer le triangle pour l'attaque
    shape.setPointCount(3);
    shape.setPoint(0, sf::Vector2f(0.0f, 0.0f));             // Sommet
    shape.setPoint(1, sf::Vector2f(-60.0f, 120.0f));          // Base gauche
    shape.setPoint(2, sf::Vector2f(60.0f, 120.0f));           // Base droite
    shape.setFillColor(sf::Color(255, 0, 0, 200));           // Rouge semi-transparent
    shape.setPosition(position);

    // Calculer la rotation pour orienter l'attaque vers la cible
    float angle = std::atan2(direction.y, direction.x) * 180.0f / M_PI - 90.f;
    shape.setRotation(angle);
}

void SwordAttack::update(float dt) {
    timeElapsed += dt;
}

void SwordAttack::render(sf::RenderWindow& window) {
    window.draw(shape);
}

bool SwordAttack::isExpired() const {
    return timeElapsed >= lifetime;
}

const sf::ConvexShape& SwordAttack::getShape() const {
    return shape;
}
