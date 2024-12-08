#ifndef SPACE_GARDENER_H
#define SPACE_GARDENER_H

#include "SFML/Graphics.hpp"

#include "scene.h"
#include "game.h"

extern std::shared_ptr<Scene> gameScene;
extern std::shared_ptr<Scene> menuScene;
extern std::shared_ptr<Scene> activeScene;

#define entity_spawn_radius 1000

class MenuScene final : public Scene {
private:
    Font font;

    Text menu_title_text;

    Text menu_start_text;
    RectangleShape menu_start_button;

    Text credit_text;

    Text menu_settings_text;
    RectangleShape menu_settings_button;

    Text menu_exit_text;
    RectangleShape menu_exit_button;

public:
    MenuScene() = default;
    void update(double dt) override;
    void render(RenderWindow& window) override;
    void load() override;
};

class MainSettingsScene final : public Scene {
private:
    Text settings_title_text;
    Text settings_video_text;
    Text settings_audio_text;
    Text settings_input_text;
    Font font;

public:
    MainSettingsScene() = default;
    void update(double dt) override;
    void render(RenderWindow& window) override;
    void load() override;
};

class GameScene final : public Scene {
private:
    Font font;
    Text respawn_text;

    Text score_text;
    RectangleShape score_overlay;

    Clock scoreClock;
    void respawn();

    RectangleShape respawnButton;

public:
    GameScene() = default;
    void update(double dt) override;
    void render(RenderWindow& window) override;
    void load() override;
};

void initScenes();

#endif // SPACE_GARDENER_H
