#include "ninja_thief.hpp"

using namespace picosystem;

// Normally, global variables should be avoided where possible, but for simplicity they will sometimes be used in this project

// Declare background, level, current_level and last_time as global variables so that we can access them in the init, update, and render functions.
buffer_t* background = nullptr;

Level level;

uint8_t current_level = 0;

// Used to calculate time since last frame
float last_time = 0.0f;

// Setup the game
void init() {
    // Set backlight to max
    backlight(100);

    // Load the spritesheet
    buffer_t* sprites = buffer(Constants::SPRITESHEET_WIDTH, Constants::SPRITESHEET_HEIGHT, asset_sprites);
    spritesheet(sprites);

    // Load the background
    background = buffer(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, asset_background);

    // Load the first level
    level = Level(0);
}

// Update the game
void update(uint32_t tick) {
    // Calculate change in time (in seconds) since last frame
    float dt = (time() - last_time) / 1000.0f;
    last_time = time();

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
void draw(uint32_t tick) {
    // Clear the screen
    pen();
    clear();

    // Draw the background
    blit(background, 0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, 0, 0);

    // Render the level, which includes drawing the player and enemies
    level.render();
}