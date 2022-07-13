#include "ninja_thief.hpp"

// Declare background as a global variable so that we can access it in both the init and render functions.
// Normally, global variables should be avoided where possible, but for simpicity they will sometimes be used in this project
Surface* background = nullptr;

Level level;

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
    level = Level(Constants::LEVELS[0]);
}

// Update the game
void update(uint32_t time) {
    // Calculate change in time (in seconds) since last frame
    float dt = (time - last_time) / 1000.0f;

    // Cap dt (for when the game is paused)
    if (dt > 0.05f) dt = 0.05f;

    level.update(dt);
}

// Render the game (draw it to the display)
void render(uint32_t time) {

    // Clear the screen
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    // Draw the background
    screen.blit(background, Rect(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT), Point(0, 0));

    // Render the level, which includes drawing the player and enemies
    level.render(&screen);
}