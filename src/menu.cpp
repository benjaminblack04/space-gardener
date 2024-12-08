//Faire setPosition sur toutes les entities et les replacer sans les recréé au chaque fois que l'on arrive sur le jeu
//Copier le .cpp et .h des entities et voir comment set une position avec comme variable X et Y
#include "menu.h"
#include "entity.h"
#include "LevelSystem.h"
#include "game.h"
#include "misc.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <iomanip>
#include <cstdlib>

#include "graphicsHelpers.h"

// Définitions des scènes
std::shared_ptr<Scene> gameScene;
std::shared_ptr<Scene> menuScene;

// Active scene
std::shared_ptr<Scene> activeScene;

View cameraView;

int enemyLastSpawned = 0;

float currentTime = 0;

void MenuScene::load() {
    if (_alreadyLoad == false) {
        _alreadyLoad = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            std::cerr << "Error: Font not found!" << std::endl;
            return; // Quitter si la police ne peut pas être chargée
        }

        const Color gris(128, 128, 200);

        setMenuButton(menu_start_button, menu_start_text, gris, Color::White, font, 28, "Press to Start", gameWidth, gameHeight, 400);
        setMenuButton(menu_settings_button, menu_settings_text, gris, Color::White, font, 28, "Settings", gameWidth, gameHeight, 200);
        setMenuButton(menu_exit_button, menu_exit_text, Color::White, Color::Red, font, 28, "Exit", gameWidth, gameHeight, 50);

        setMenuText(menu_title_text, font, 50, Color::Yellow, "Space Gardener", gameWidth, gameHeight, 500);
        setMenuText(credit_text, font, 14, Color::White, "Benjamin & Thibault", gameWidth, gameHeight, 300);
    }

    cameraView.setSize(gameWidth, gameHeight);
    cameraView.setCenter(gameWidth * 0.5f, gameHeight * 0.5f);
}

void MenuScene::update(double dt) {
    Scene::update(dt);

    if (Mouse::isButtonPressed(Mouse::Left)) {
        const Vector2i relativeMousePosition = Mouse::getPosition(*window); // Position de la souris
        const Vector2f globalMousePosition = window->mapPixelToCoords(relativeMousePosition);

        if (menu_start_button.getGlobalBounds().contains(globalMousePosition)) {
            activeScene = gameScene;
            activeScene->load();
        }

        if (menu_exit_button.getGlobalBounds().contains(globalMousePosition))
            window->close();

    }
}

void MenuScene::render(RenderWindow& window) {
    // Set up camera view to render the menu
    window.setView(cameraView);

    // Game title
    window.draw(menu_title_text);

    // Draw the start button
    window.draw(menu_start_button);
    window.draw(menu_start_text);

    // Game Credits
    window.draw(credit_text);

    // Game settings
    window.draw(menu_settings_button);
    window.draw(menu_settings_text);

    // Exit button
    window.draw(menu_exit_button);
    window.draw(menu_exit_text);

    // Draw the scene
    // Note: button must be drawn first, as to not be on top of the text/label.
    Scene::render(window);
}

void GameScene::load() {
    if (_alreadyLoad == false) {
        _alreadyLoad = true;

        cameraView.setSize(gameWidth, gameHeight);
        cameraView.setCenter(gameWidth * .5f, gameHeight * .5f);

        // Charger la police pour le texte du score
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            std::cerr << "Error: Font not found!" << std::endl;
            return;
        }

        // Créer le joueur et les fantômes, puis les ajouter à l'EntityManager de la scène
        if (_ents.list.empty()) {
            const auto player = std::make_shared<Player>();
            player->setPosition({150, 650});
            _ents.list.push_back(player);

            try {
                int i = 0;
                auto wall = ls::getObject(i);

                while (wall.getScale().x > 0) {
                    player->addWall(wall);
                    i++;
                    wall = ls::getObject(i);
                }
            } catch(exception e) {
                return;
            }

            return;
        }
    }

    // Démarrer l'horloge de score
    scoreClock.restart();
    // reset the score too
}

void GameScene::update(double dt) {
    // Mise à jour normale des entités
    Scene::update(dt);

    auto player = getPlayer();
    if (!player) return;

    // Si le joueur est mort, attendre une action de respawn
    if (player->isAlive() == false) {
        if (Keyboard::isKeyPressed(Keyboard::R)) {
            respawn();
        }
        if (Mouse::isButtonPressed(Mouse::Left)) {
            Vector2i mousePos = Mouse::getPosition(*window); // Position de la souris
            Vector2f worldPos = window->mapPixelToCoords(mousePos);
            if (respawnButton.getGlobalBounds().contains(worldPos)) {
                respawn();
            }
        }
        return; // Ne pas mettre à jour les autres entités
    }

    int currentTime = static_cast<int>(scoreClock.getElapsedTime().asSeconds());
    currentTime = currentTime == 0 ? 1 : currentTime;

    if (currentTime % 3 == 0 && enemyLastSpawned == currentTime - 3) {
        enemyLastSpawned = currentTime;

        auto enemy = std::make_shared<Enemy>();
        enemy->getPlayer(player);
        enemy->setPosition({static_cast<float>(rand() % entity_spawn_radius), static_cast<float>(rand() % entity_spawn_radius)});
        _ents.list.push_back(enemy);
    }

    cameraView.setCenter(player->getPosition());

    if (!window) {
        std::cout << "window don't find !! " << std::endl;
    }
    player->handleMouseInput(*window);
}

void GameScene::render(RenderWindow& window) {
    window.setView(cameraView); // Appliquer la vue de la caméra

    auto player = getPlayer();
    if (!player) return;

     // Calculer les limites visibles de la caméra
    Vector2f viewTopLeft = cameraView.getCenter() - cameraView.getSize() / 2.0f;
    Vector2f viewBottomRight = cameraView.getCenter() + cameraView.getSize() / 2.0f;

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
    Text time_text;
    time_text.setFont(font);
    time_text.setCharacterSize(24);
    time_text.setFillColor(Color::White);
    time_text.setString("Time: " + std::to_string(round(currentTime * 100) / 100));
    time_text.setPosition(cameraView.getCenter().x - (time_text.getLocalBounds().getSize().x / 2),
                          cameraView.getCenter().y + 250);

    RectangleShape time_overlay({floor(time_text.getLocalBounds().getSize().x + 10),
                                          45});
    time_overlay.setFillColor(Color(0, 0, 0, 190)); // Couleur semi-transparente
    time_overlay.setPosition(time_text.getPosition().x - 5, time_text.getPosition().y - 5);

    window.draw(time_overlay);
    window.draw(time_text);

    #ifdef __DEBUG_SHOW_WALLS__
    try {
        int i = 0;
        auto wall = ls::getObject(i);

        while (wall.getScale().x > 0) {
            window.draw(wall);
            i++;
            wall = ls::getObject(i);
        }
    } catch(exception e) {

    }
    #endif

    // Kill count
    Text kills_text;
    kills_text.setFont(font);
    kills_text.setCharacterSize(24);
    kills_text.setFillColor(Color::White);
    kills_text.setString("Kills: " + std::to_string(_ents.score));
    kills_text.setPosition(cameraView.getCenter().x - (kills_text.getLocalBounds().getSize().x / 2), cameraView.getCenter().y + 200); //cameraView.getCenter().x ), cameraView.getCenter().y + 200

    RectangleShape kills_overlay({floor(kills_text.getLocalBounds().getSize().x + 10),
                                          45});
    kills_overlay.setFillColor(Color(0, 0, 0, 190)); // Couleur semi-transparente
    kills_overlay.setPosition(kills_text.getPosition().x - 5, kills_text.getPosition().y - 5);

    window.draw(kills_overlay);
    window.draw(kills_text);

    // Si le joueur est mort, ajouter un overlay semi-transparent
    if (!player->isAlive()) {
        RectangleShape overlay(Vector2f(cameraView.getSize().x, cameraView.getSize().y));
        overlay.setFillColor(Color(40, 15, 15, 225)); // Couleur semi-transparente
        overlay.setPosition(cameraView.getCenter().x - cameraView.getSize().x / 2.0f,
                            cameraView.getCenter().y - cameraView.getSize().y / 2.0f);

        window.draw(overlay);

        Text respawnText;
        respawnText.setFont(font);
        respawnText.setCharacterSize(50);
        respawnText.setFillColor(Color::White);
        respawnText.setString("You Died!");
        respawnText.setPosition(cameraView.getCenter().x - (respawnText.getLocalBounds().getSize().x / 2), cameraView.getCenter().y - gameHeight * .3f);
        window.draw(respawnText);

        Text scoreText;
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setString("Your score is : " + std::to_string(_ents.score));
        scoreText.setPosition(cameraView.getCenter().x - (scoreText.getLocalBounds().getSize().x / 2), cameraView.getCenter().y);
        window.draw(scoreText);

        Color red(100, 40, 40);

        float respawnButton_width = 250.f;
        float respawnButton_height = 100.f;
        respawnButton.setSize(Vector2f(respawnButton_width, respawnButton_height));
        respawnButton.setOrigin(respawnButton_width * .5f, respawnButton_height * .5f);
        respawnButton.setFillColor(red);
        respawnButton.setPosition(cameraView.getCenter().x, cameraView.getCenter().y + gameHeight * .3f);
        window.draw(respawnButton);

        Text respawnButtonText;
        respawnButtonText.setFont(font);
        respawnButtonText.setCharacterSize(24);
        respawnButtonText.setFillColor(Color::White);
        respawnButtonText.setString("RESPAWN");
        respawnButtonText.setPosition(cameraView.getCenter().x - (respawnButtonText.getLocalBounds().getSize().x / 2), cameraView.getCenter().y + gameHeight * .3f - (respawnButtonText.getLocalBounds().getSize().y / 2));
        window.draw(respawnButtonText);

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
    menuScene = std::make_shared<MenuScene>();
    gameScene = std::make_shared<GameScene>();
    activeScene = menuScene; // Par défaut, commence par la scène de menu
}
