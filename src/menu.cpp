//Faire setPosition sur toutes les entities et les replacer sans les recréé au chaque fois que l'on arrive sur le jeu
//Copier le .cpp et .h des entities et voir comment set une position avec comme variable X et Y
#include "menu.h"
#include "entity.h"
#include "LevelSystem.h"
#include "game.h"
#include <iomanip>

// Définitions des scènes
std::shared_ptr<Scene> gameScene;
std::shared_ptr<Scene> menuScene;
std::shared_ptr<Scene> activeScene;

sf::View cameraView;

int enemyLastSpawned = 0;

float currentTime = 0;

int enemiesKilled = 0;

void MenuScene::load() {
    if (_alreadyLoad == false) {
        _alreadyLoad = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            std::cerr << "Error: Font not found!" << std::endl;
            return; // Quitter si la police ne peut pas être chargée

        }
        menu_text.setFont(font);
        menu_text.setCharacterSize(48);
        menu_text.setFillColor(sf::Color::Yellow);
        menu_text.setString("Space Gardener");  // Texte initial
        menu_text.setPosition((gameWidth / 2) - (menu_text.getLocalBounds().width / 2), 150);       // Position du texte

        menu_start_text.setFont(font);
        menu_start_text.setCharacterSize(29);
        menu_start_text.setFillColor(sf::Color::White);
        menu_start_text.setString("           Press [ENTER] to start\nPress [ESCAPE] to close the game");
        menu_start_text.setPosition((gameWidth / 2) - (menu_start_text.getLocalBounds().width / 2), 400);
    }

    cameraView.setSize(gameWidth, gameHeight);
    cameraView.setCenter(gameWidth * .5f, gameHeight * .5f);
}

void MenuScene::update(double dt) {
    Scene::update(dt);
}

void MenuScene::render(sf::RenderWindow& window) {
    window.setView(cameraView);
    window.draw(menu_text);   // Afficher le texte du menu
    window.draw(menu_start_text);
    Scene::render(window);  // Rendu des autres entités si besoin
}

void GameScene::load() {
    if (_alreadyLoad == false) {
        _alreadyLoad = true;

        cameraView.setSize(gameWidth, gameHeight);
        cameraView.setCenter(gameWidth * .5f, gameHeight * .5f);

        // Charger la police pour le texte du score
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            // Gérer l'erreur de chargement de la police
        }
        // Configurer le texte de respawn
        respawnText.setFont(font);
        respawnText.setCharacterSize(24);
        respawnText.setFillColor(sf::Color::White);
        respawnText.setString("You Died! Press R to Respawn");
        respawnText.setPosition(gameWidth * 0.5f - 150, gameHeight * 0.5f - 20);

        // Créer le joueur et les fantômes, puis les ajouter à l'EntityManager de la scène
        if (_ents.list.empty()) {
            auto player = std::make_shared<Player>();
            player->setPosition({150, 650});
            _ents.list.push_back(player);

            return;
        }
    }
    // Démarrer l'horloge de score
    scoreClock.restart();
}

void GameScene::update(double dt) {
    // Mise à jour normale des entités
    Scene::update(dt);

    auto player = getPlayer();
    if (!player) return;

    // Si le joueur est mort, attendre une action de respawn
    if (player->isAlive() == false) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            respawn();
        }
        return; // Ne pas mettre à jour les autres entités
    } else {
        int currentTime = static_cast<int>(scoreClock.getElapsedTime().asSeconds());
        currentTime = (currentTime == 0) ? 1 : currentTime;

        if (currentTime % 3 == 0 && enemyLastSpawned == currentTime - 3) {
            enemyLastSpawned = currentTime;

            auto enemy = std::make_shared<Enemy>();
            enemy->getPlayer(player);
            enemy->setPosition({rand() % 1000, rand() % 1000}); //rand() % gameWidth, rand() % gameHeight
            _ents.list.push_back(enemy);
        }
    }

    cameraView.setCenter(player->getPosition());

    if (!window) {
        std::cout << "window don't find !! " << std::endl;
    }
    player->handleMouseInput(*window);
}

void GameScene::render(sf::RenderWindow& window) {
    window.setView(cameraView); // Appliquer la vue de la caméra

    auto player = getPlayer();
    if (!player) return;

     // Calculer les limites visibles de la caméra
    sf::Vector2f viewTopLeft = cameraView.getCenter() - cameraView.getSize() / 2.0f;
    sf::Vector2f viewBottomRight = cameraView.getCenter() + cameraView.getSize() / 2.0f;

    ls::Render(window);

    // Dessiner les barres de vie des ennemis
    for (const auto& entity : _ents.list) {
        if (auto enemy = std::dynamic_pointer_cast<Enemy>(entity)) {
            if (enemy->isAlive()) {
                enemy->drawHealthBar(window);
            }
        }
    }

    // Dessiner la barre de vie du joueur
    player->drawHealthBar(window, font);
    player->drawSaminaBar(window, font);

    // Dessiner les entités elles-mêmes
    Scene::render(window);

    // TODO: round down
    sf::Text time_text;
    time_text.setFont(font);
    time_text.setCharacterSize(24);
    time_text.setFillColor(sf::Color::White);
    time_text.setString("Time: " + std::to_string(round(currentTime * 100) / 100));
    time_text.setPosition(cameraView.getCenter().x - (time_text.getLocalBounds().getSize().x / 2),
                          cameraView.getCenter().y + 250);

    sf::RectangleShape time_overlay({floor(time_text.getLocalBounds().getSize().x + 10),
                                          45});
    time_overlay.setFillColor(sf::Color(0, 0, 0, 190)); // Couleur semi-transparente
    time_overlay.setPosition(time_text.getPosition().x - 5, time_text.getPosition().y - 5);

    window.draw(time_overlay);
    window.draw(time_text);

    // Kill count
    sf::Text kills_text;
    kills_text.setFont(font);
    kills_text.setCharacterSize(24);
    kills_text.setFillColor(sf::Color::White);
    kills_text.setString("Kills: " + std::to_string(enemiesKilled));
    kills_text.setPosition(cameraView.getCenter().x - (kills_text.getLocalBounds().getSize().x / 2),
                          cameraView.getCenter().y + 200);

    sf::RectangleShape kills_overlay({floor(kills_text.getLocalBounds().getSize().x + 10),
                                          45});
    kills_overlay.setFillColor(sf::Color(0, 0, 0, 190)); // Couleur semi-transparente
    kills_overlay.setPosition(kills_text.getPosition().x - 5, kills_text.getPosition().y - 5);

    window.draw(kills_overlay);
    window.draw(kills_text);

    // Si le joueur est mort, ajouter un overlay semi-transparent
    if (!player->isAlive()) {
        sf::RectangleShape overlay(sf::Vector2f(cameraView.getSize().x, cameraView.getSize().y));
        overlay.setFillColor(sf::Color(40, 15, 15, 225)); // Couleur semi-transparente
        overlay.setPosition(cameraView.getCenter().x - cameraView.getSize().x / 2.0f,
                            cameraView.getCenter().y - cameraView.getSize().y / 2.0f);

        window.draw(overlay);

        sf::Text respawnText;
        respawnText.setFont(font);
        respawnText.setCharacterSize(24);
        respawnText.setFillColor(sf::Color::White);
        respawnText.setString("You Died! Press R to Respawn");
        respawnText.setPosition(cameraView.getCenter().x - 170, cameraView.getCenter().y - 20);
        window.draw(respawnText);
    } else {
        currentTime = scoreClock.getElapsedTime().asSeconds();
    }
}

void GameScene::respawn() {
    _ents.list.clear();

    scoreClock.restart();
    enemyLastSpawned = 0;

    auto player = std::make_shared<Player>();
    player->setPosition({150, 650});
    _ents.list.push_back(player);
}

// Initialiser les scènes
void initScenes() {
    gameScene = std::make_shared<GameScene>();
    menuScene = std::make_shared<MenuScene>();
    activeScene = menuScene; // Par défaut, commence par la scène de menu
}
