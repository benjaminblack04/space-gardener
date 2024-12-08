#ifndef SPACE_GARDENER_H
#define SPACE_GARDENER_H

#include "SFML/Graphics.hpp"

#include "scene.h"
#include "game.h"

extern std::shared_ptr<Scene> menuScene;

extern std::shared_ptr<Scene> mainSettingsScene;
extern std::shared_ptr<Scene> videoSettingsScene;
extern std::shared_ptr<Scene> audioSettingsScene;
extern std::shared_ptr<Scene> inputSettingsScene;

extern std::shared_ptr<Scene> gameScene;

extern std::shared_ptr<Scene> activeScene;

extern bool leftMouseButtonPressed;

#define entity_spawn_radius 1000

class MenuScene final : public Scene {
private:
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
    RectangleShape settings_video_button;

    Text settings_audio_text;
    RectangleShape settings_audio_button;

    Text settings_input_text;
    RectangleShape settings_input_button;

    Text settings_back_text;
    RectangleShape settings_back_button;
public:
    MainSettingsScene() = default;
    void update(double dt) override;
    void render(RenderWindow& window) override;
    void load() override;
};

class VideoSettingsScene final : public Scene {
private:
    Text settings_title_text;

    Text settings_back_text;
    RectangleShape settings_back_button;
public:
    VideoSettingsScene() = default;
    void update(double dt) override;
    void render(RenderWindow& window) override;
    void load() override;
};

class AudioSettingsScene final : public Scene {
private:
    Text settings_title_text;

    Text settings_back_text;
    RectangleShape settings_back_button;
public:
    AudioSettingsScene() = default;
    void update(double dt) override;
    void render(RenderWindow& window) override;
    void load() override;
};

class InputSettingsScene final : public Scene {
private:
    Text settings_title_text;

    Text settings_back_text;
    RectangleShape settings_back_button;
public:
    InputSettingsScene() = default;
    void update(double dt) override;
    void render(RenderWindow& window) override;
    void load() override;
};

class GameScene final : public Scene {
private:
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
