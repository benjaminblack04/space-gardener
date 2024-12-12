#include "sceneManager.h"
#include "LevelSystem.h"
#include "game.h"
#include "graphicsHelpers.h"

#include <iostream>

// Définitions des scènes
shared_ptr<Scene> menuScene;

shared_ptr<Scene> mainSettingsScene;
shared_ptr<Scene> videoSettingsScene;
shared_ptr<Scene> audioSettingsScene;
shared_ptr<Scene> inputSettingsScene;

shared_ptr<Scene> gameScene;

// Active scene
shared_ptr<Scene> activeScene;

View cameraView;

Music backgroundMusic;
shared_ptr<Sound> audioSound;

int enemyLastSpawned = 0;

float currentTime = 0;

void MenuScene::load() {
    // Play menu music
    backgroundMusic.stop();
    backgroundMusic.openFromFile("res/mus/menu.wav");
    backgroundMusic.play();

    if (_alreadyLoaded == false) {
        _alreadyLoaded = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            cerr << "Error: Font not found!" << endl;
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

    if (Mouse::isButtonPressed(Mouse::Left) && leftMouseButtonPressed && tabbedIn) {
        leftMouseButtonPressed = false;
        const Vector2i relativeMousePosition = Mouse::getPosition(*window); // Position de la souris
        const Vector2f globalMousePosition = window->mapPixelToCoords(relativeMousePosition);

        if (menu_start_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load game scene" << endl;
            activeScene = gameScene;
            activeScene->load();
        }

        if (menu_settings_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load settings scene" << endl;
            activeScene = mainSettingsScene;
            activeScene->load();
        }

        if (menu_exit_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to close game" << endl;
            window->close();
        }

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

void MainSettingsScene::load() {
    if (_alreadyLoaded == false) {
        _alreadyLoaded = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            cerr << "Error: Font not found!" << endl;
            return; // Quitter si la police ne peut pas être chargée
        }

        const Color gris(128, 128, 200);

        setMenuButton(settings_audio_button, settings_audio_text, gris, Color::White, font, 28, "Audio Settings", gameWidth, gameHeight, 300);

        setMenuButton(settings_back_button, settings_back_text, Color::White, Color::Red, font, 28, "Back", gameWidth, gameHeight, 50);

        setMenuText(settings_title_text, font, 50, Color::Yellow, "Space Gardener: Settings", gameWidth, gameHeight, 500);
    }

    cameraView.setSize(gameWidth, gameHeight);
    cameraView.setCenter(gameWidth * 0.5f, gameHeight * 0.5f);
}

void MainSettingsScene::update(const double dt) {
    Scene::update(dt);

    if (Mouse::isButtonPressed(Mouse::Left) && leftMouseButtonPressed && tabbedIn) {
        leftMouseButtonPressed = false;
        const Vector2i relativeMousePosition = Mouse::getPosition(*window); // Position de la souris
        const Vector2f globalMousePosition = window->mapPixelToCoords(relativeMousePosition);

        if (settings_video_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load video settings scene" << endl;
            activeScene = videoSettingsScene;
            activeScene->load();
        }

        if (settings_audio_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load audio settings scene" << endl;
            activeScene = audioSettingsScene;
            activeScene->load();
        }

        if (settings_input_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load input settings scene" << endl;
            activeScene = inputSettingsScene;
            activeScene->load();
        }

        if (settings_back_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load menu scene" << endl;
            activeScene = menuScene;
            activeScene->load();
        }
    }
}

void MainSettingsScene::render(RenderWindow& window) {
    // Set up camera view to render the menu
    window.setView(cameraView);

    // Game title
    window.draw(settings_title_text);

    // Video settings
    window.draw(settings_video_button);
    window.draw(settings_video_text);

    // Audio settings
    window.draw(settings_audio_button);
    window.draw(settings_audio_text);

    // Input settings
    window.draw(settings_input_button);
    window.draw(settings_input_text);

    // Back button
    window.draw(settings_back_button);
    window.draw(settings_back_text);

    // Draw the scene
    // Note: button must be drawn first, as to not be on top of the text/label.
    Scene::render(window);
}

void VideoSettingsScene::load() {
    if (_alreadyLoaded == false) {
        _alreadyLoaded = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            cerr << "Error: Font not found!" << endl;
            return; // Quitter si la police ne peut pas être chargée
        }

        setMenuButton(settings_back_button, settings_back_text, Color::White, Color::Red, font, 28, "Back", gameWidth, gameHeight, 50);

        setMenuText(resolution_text, font, 28, Color::White, "Resolution", gameWidth, gameHeight, 400);



        setMenuText(settings_title_text, font, 50, Color::Yellow, "Space Gardener: Video Settings", gameWidth, gameHeight, 500);
    }

    cameraView.setSize(gameWidth, gameHeight);
    cameraView.setCenter(gameWidth * 0.5f, gameHeight * 0.5f);
}

void VideoSettingsScene::update(const double dt) {
    Scene::update(dt);

    if (Mouse::isButtonPressed(Mouse::Left) && leftMouseButtonPressed && tabbedIn) {
        leftMouseButtonPressed = false;
        const Vector2i relativeMousePosition = Mouse::getPosition(*window); // Position de la souris
        const Vector2f globalMousePosition = window->mapPixelToCoords(relativeMousePosition);

        if (settings_back_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load settings scene" << endl;
            activeScene = mainSettingsScene;
            activeScene->load();
        }
    }
}

void VideoSettingsScene::render(RenderWindow& window) {
    // Set up camera view to render the menu
    window.setView(cameraView);

    // Game title
    window.draw(settings_title_text);

    // Resolution slider
    window.draw(resolution_text);
    //TODO:

    // Back button
    window.draw(settings_back_button);
    window.draw(settings_back_text);

    // Draw the scene
    // Note: button must be drawn first, as to not be on top of the text/label.
    Scene::render(window);
}

void AudioSettingsScene::load() {
    if (_alreadyLoaded == false) {
        _alreadyLoaded = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            cerr << "Error: Font not found!" << endl;
            return; // Quitter si la police ne peut pas être chargée
        }

        setMenuButton(settings_back_button, settings_back_text, Color::White, Color::Red, font, 28, "Back", gameWidth, gameHeight, 50);

        setMenuText(music_volume_text, font, 28, Color::White, "Music Volume", gameWidth, gameHeight, 450);
        music_volume_slider.configure(gameWidth * 0.5f, gameHeight * 0.5f - 100);
        music_volume_slider.create(0, 100);
        music_volume_slider.setSliderValue(50);

        setMenuText(audio_volume_text, font, 28, Color::White, "Audio Volume", gameWidth, gameHeight, 300);
        audio_volume_slider.configure(gameWidth * 0.5f, gameHeight * 0.5f + 50);
        audio_volume_slider.create(0, 100);
        audio_volume_slider.setSliderValue(50);

        setMenuText(settings_title_text, font, 50, Color::Yellow, "Space Gardener: Audio Settings", gameWidth, gameHeight, 500);
    }

    cameraView.setSize(gameWidth, gameHeight);
    cameraView.setCenter(gameWidth * 0.5f, gameHeight * 0.5f);
}

void AudioSettingsScene::update(const double dt) {
    Scene::update(dt);

    if (Mouse::isButtonPressed(Mouse::Left) && leftMouseButtonPressed && tabbedIn) {
        leftMouseButtonPressed = false;
        const Vector2i relativeMousePosition = Mouse::getPosition(*window); // Position de la souris
        const Vector2f globalMousePosition = window->mapPixelToCoords(relativeMousePosition);

        if (settings_back_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load settings scene" << endl;
            activeScene = mainSettingsScene;
            activeScene->load();
        }
    }
    backgroundMusic.setVolume(music_volume_slider.getSliderValue());
    audioSound->setVolume(audio_volume_slider.getSliderValue());

}

void AudioSettingsScene::render(RenderWindow& window) {
    // Set up camera view to render the menu
    window.setView(cameraView);

    // Game title
    window.draw(settings_title_text);

    // Music Volume slider
    window.draw(music_volume_text);
    music_volume_slider.draw(window);

    // Audio Volume slider
    window.draw(audio_volume_text);
    audio_volume_slider.draw(window);

    // Back button
    window.draw(settings_back_button);
    window.draw(settings_back_text);

    // Draw the scene
    // Note: button must be drawn first, as to not be on top of the text/label.
    Scene::render(window);
}

void InputSettingsScene::load() {
    if (_alreadyLoaded == false) {
        _alreadyLoaded = true;
        // Chargement de la police (assurez-vous d'avoir un fichier de police accessible)
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            cerr << "Error: Font not found!" << endl;
            return; // Quitter si la police ne peut pas être chargée
        }

        setMenuButton(settings_back_button, settings_back_text, Color::White, Color::Red, font, 28, "Back", gameWidth, gameHeight, 50);

        // TODO: What should be here?

        setMenuText(settings_title_text, font, 50, Color::Yellow, "Space Gardener: Input Settings", gameWidth, gameHeight, 500);
    }

    cameraView.setSize(gameWidth, gameHeight);
    cameraView.setCenter(gameWidth * 0.5f, gameHeight * 0.5f);
}

void InputSettingsScene::update(const double dt) {
    Scene::update(dt);

    if (Mouse::isButtonPressed(Mouse::Left) && leftMouseButtonPressed && tabbedIn) {
        leftMouseButtonPressed = false;
        const Vector2i relativeMousePosition = Mouse::getPosition(*window); // Position de la souris
        const Vector2f globalMousePosition = window->mapPixelToCoords(relativeMousePosition);

        if (settings_back_button.getGlobalBounds().contains(globalMousePosition)) {
            clog << "[Update] Trying to load settings scene" << endl;
            activeScene = mainSettingsScene;
            activeScene->load();
        }
    }
}

void InputSettingsScene::render(RenderWindow& window) {
    // Set up camera view to render the menu
    window.setView(cameraView);

    // Game title
    window.draw(settings_title_text);

    // Back button
    window.draw(settings_back_button);
    window.draw(settings_back_text);

    // Draw the scene
    // Note: button must be drawn first, as to not be on top of the text/label.
    Scene::render(window);
}

void GameScene::load() {
    // Play menu music
    backgroundMusic.stop();
    backgroundMusic.openFromFile("res/mus/battle.wav");
    backgroundMusic.play();

    // Load background
    _backgroundTexture.loadFromFile("res/background.png");
    _backgroundSprite.setTexture(_backgroundTexture);
    _backgroundSprite.setPosition(-1000, -200);
    _backgroundSprite.setScale({2, 2});

    if (_alreadyLoaded == false) {
        _alreadyLoaded = true;

        cameraView.setSize(gameWidth, gameHeight);
        cameraView.setCenter(gameWidth * .5f, gameHeight * .5f);

        // Charger la police pour le texte du score
        if (!font.loadFromFile("./res/fonts/Arial.ttf")) {
            cerr << "Error: Font not found!" << endl;
            return;
        }

        // Créer le joueur et les fantômes, puis les ajouter à l'EntityManager de la scène
        if (_entities.list.empty()) {
            const auto player = make_shared<Player>();
            player->getSound(audioSound);
            player->setPosition({150, 650});
            _entities.list.push_back(player);

            try {
                int i = 0;
                auto wall = ls::getObject(i);

                while (wall.getScale().x > 0) {
                    player->addWall(wall);
                    i++;
                    wall = ls::getObject(i);
                }
            } catch(length_error& e) {
                cerr << e.what() << endl;
            }
        }
    }
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
        if (Mouse::isButtonPressed(Mouse::Left) && leftMouseButtonPressed) {
        leftMouseButtonPressed = false;
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

        auto enemy = make_shared<Enemy>();
        enemy->getSound(audioSound);
        enemy->getPlayer(player);
        enemy->setPosition({
            static_cast<float>(rand() % entity_spawn_radius),
            static_cast<float>(rand() % entity_spawn_radius)
        });
        _entities.list.push_back(enemy);
    }

    cameraView.setCenter(player->getPosition());

    if (!window) {
        cout << "[Warning] Can't find Window " << endl;
    }
    player->handleMouseInput(*window);
}

void GameScene::render(RenderWindow& window) {
    window.setView(cameraView); // Appliquer la vue de la caméra

    window.draw(_backgroundSprite);

    auto player = getPlayer();
    if (!player) return;

     // Calculer les limites visibles de la caméra
    Vector2f viewTopLeft = cameraView.getCenter() - cameraView.getSize() / 2.0f;
    Vector2f viewBottomRight = cameraView.getCenter() + cameraView.getSize() / 2.0f;

    ls::Render(window);

    // Dessiner les barres de vie des ennemis
    for (const auto& entity : _entities.list) {
        if (auto enemy = dynamic_pointer_cast<Enemy>(entity)) {
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
    time_text.setString("Time: " + to_string(round(currentTime * 100) / 100));
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
        std:cerr << "Error: " << e.what() << endl;
    }
    #endif

    // Kill count
    Text kills_text;
    kills_text.setFont(font);
    kills_text.setCharacterSize(24);
    kills_text.setFillColor(Color::White);
    kills_text.setString("Kills: " + to_string(_entities.score));
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
        scoreText.setString("Your score is : " + to_string(_entities.score));
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
    _entities.list.clear();

    scoreClock.restart();
    enemyLastSpawned = 0;

    auto player = make_shared<Player>();
    player->setPosition({150, 650});
    _entities.list.push_back(player);
}

void initScenes() {
    backgroundMusic.setLoop(true);
    audioSound = make_shared<Sound>();

    // main menu scene
    menuScene = make_shared<MenuScene>();

    // specific menu scenes
    mainSettingsScene = make_shared<MainSettingsScene>();
    videoSettingsScene = make_shared<VideoSettingsScene>();
    audioSettingsScene = make_shared<AudioSettingsScene>();
    inputSettingsScene = make_shared<InputSettingsScene>();

    // game scene
    gameScene = make_shared<GameScene>();

    // Set the active scene to the menu
    activeScene = menuScene;
}
