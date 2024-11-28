//Faire setPosition sur toutes les entities et les replacer sans les recréé au chaque fois que l'on arrive sur le jeu
//Copier le .cpp et .h des entities et voir comment set une position avec comme variable X et Y
#include "menu.h"
#include "entity.h"

// Définitions des scènes
std::shared_ptr<Scene> gameScene;
std::shared_ptr<Scene> menuScene;
std::shared_ptr<Scene> activeScene;

sf::View cameraView;

void MenuScene::load() {

    if (_alreadyLoad == false) {
        _alreadyLoad = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            std::cerr << "Error: Font not found!" << std::endl;
            return; // Quitter si la police ne peut pas être chargée

        }
        text.setFont(font);
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::Yellow);
        text.setString("Space Gardener");  // Texte initial
        text.setPosition(200, 250);       // Position du texte

    }
    cameraView.setSize(gameWidth, gameHeight);
    cameraView.setCenter(gameWidth * .5f, gameHeight * .5f);
}

void MenuScene::update(double dt) {
    Scene::update(dt);
    //text.setString("Almost Pacman");
}

void MenuScene::render(sf::RenderWindow& window) {
    window.setView(cameraView);
    window.draw(text);   // Afficher le texte du menu
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
            player->setPosition(sf::Vector2f(400, 300));
            _ents.list.push_back(player);

            for (int i = 0; i < 16; ++i) {
                auto enemy = std::make_shared<Enemy>();
                enemy->getPlayer(player);
                enemy->setPosition(sf::Vector2f(100, 100)); //rand() % gameWidth, rand() % gameHeight
                _ents.list.push_back(enemy);
            }
        }
    }
    // Démarrer l'horloge de score
    scoreClock.restart();
}

void GameScene::update(double dt) {
    // Mise à jour normale des entités
    Scene::update(dt);

    // Mettre à jour le texte du score en fonction du temps écoulé
    /*int score = static_cast<int>(scoreClock.getElapsedTime().asSeconds());
    text.setString("Score: " + std::to_string(score));*/


    auto player = getPlayer();
    if (!player) return;

    // Si le joueur est mort, attendre une action de respawn
    if (player->isAlive() == false) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            respawn();
        }
        return; // Ne pas mettre à jour les autres entités
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

    //std::cout << "window don't find !! " << std::endl;
    // Effacer l'écran avec une couleur de fond
    window.clear(sf::Color::Black);

    // Dessiner la grille fixe dans le monde
    sf::VertexArray grid(sf::Lines);
    int gridSpacing = 50; // Taille de chaque cellule

    // Calculer les limites visibles de la caméra
    sf::Vector2f viewTopLeft = cameraView.getCenter() - cameraView.getSize() / 2.0f;
    sf::Vector2f viewBottomRight = cameraView.getCenter() + cameraView.getSize() / 2.0f;

    // Lignes verticales
    for (int x = static_cast<int>(viewTopLeft.x) / gridSpacing * gridSpacing; x <= viewBottomRight.x; x += gridSpacing) {
        grid.append(sf::Vertex(sf::Vector2f(x, viewTopLeft.y), sf::Color(128, 128, 128)));
        grid.append(sf::Vertex(sf::Vector2f(x, viewBottomRight.y), sf::Color(128, 128, 128)));
    }

    // Lignes horizontales
    for (int y = static_cast<int>(viewTopLeft.y) / gridSpacing * gridSpacing; y <= viewBottomRight.y; y += gridSpacing) {
        grid.append(sf::Vertex(sf::Vector2f(viewTopLeft.x, y), sf::Color(128, 128, 128)));
        grid.append(sf::Vertex(sf::Vector2f(viewBottomRight.x, y), sf::Color(128, 128, 128)));
    }

    window.draw(grid); // Dessiner la grille

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
    }
}


void GameScene::respawn() {
    for (auto& entity : _ents.list) {
        if (auto player = std::dynamic_pointer_cast<Player>(entity)) {
            // Réinitialiser le joueur
            player->setAlive(true);
            //player->getShape().setFillColor(sf::Color::Red);
            player->setHealth(player->getMaxHealth()); // Réinitialiser les points de vie à leur maximum
            player->setPosition(sf::Vector2f(gameWidth * 0.5f, gameHeight * 0.5f)); // Position centrale
            player->syncShapePosition();
            //player->getShape()->setFillColor(sf::Color::Yellow);
            cameraView.setCenter(player->getPosition());
        }
        else if (auto enemy = std::dynamic_pointer_cast<Enemy>(entity)) {
            // Réinitialiser les ennemis
            enemy->setAlive(true);
            enemy->setHealth(enemy->getMaxHealth()); // Réinitialiser les points de vie
            enemy->setPosition(sf::Vector2f(rand() % gameWidth, rand() % gameHeight)); // Position aléatoire
            enemy->syncShapePosition();

            enemy->clearAttacks();
        }
    }
}

// Initialiser les scènes
void initScenes() {
    gameScene = std::make_shared<GameScene>();
    menuScene = std::make_shared<MenuScene>();
    activeScene = menuScene; // Par défaut, commence par la scène de menu
}
