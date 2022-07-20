#include "ninja_thief.hpp"

using namespace blit;

// Normally, global variables should be avoided where possible, but for simplicity they will sometimes be used in this project

// Declare background, level, current_level and last_time as global variables so that we can access them in the init, update, and render functions.
Surface* background = nullptr;

Level level;

uint8_t current_level = 0;

// Used to calculate time since last frame
float last_time = 0.0f;

// Setup the game
void init() {
    // Set screen to 160x120 mode
    set_screen_mode(ScreenMode::lores);

    // Load the spritesheet from assets.cpp
    screen.sprites = Surface::load(asset_sprites);

    // Load the background from assets.cpp
    background = Surface::load(asset_background);

    // Load the first level
    level = Level(0);
}

// Update the game
void update(uint32_t time) {
    // Calculate change in time (in seconds) since last frame
    float dt = (time - last_time) / 1000.0f;
    last_time = time;

    // Cap dt (in case the game is paused/lagging)
    if (dt > 0.05f) {
        dt = 0.05f;
    }

    level.update(dt);

    if (level.level_failed()) {
        // Restart the same level
        level = Level(current_level);
    }
    else if (level.level_complete()) {
        // Start the next level
        current_level++;
        current_level %= Constants::LEVEL_COUNT;

        level = Level(current_level);
    }
}

// Render the game (draw it to the display)
void render(uint32_t time) {

    // Clear the screen
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    // Draw the background
    screen.blit(background, Rect(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT), Point(0, 0));

    // Render the level, which includes drawing the player and enemies
    level.render();
}