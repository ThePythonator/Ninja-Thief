#include "ninja_thief.hpp"

// Declare background as a global variable so that we can access it in both the init and render functions.
// Normally, global variables should be avoided where possible, but for simpicity they will sometimes be used in this project
Surface* background = nullptr;

Level level;

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

}

// Render the game (draw it to the display)
void render(uint32_t time) {

    // Clear the screen
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    // Draw the background
    screen.blit(background, Rect(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT), Point(0, 0));

    // Render the level
    level.render(&screen);

    // Draw the player
    screen.sprite(Constants::Sprites::PLAYER_IDLE, Point(10, 20));


    // Draw some text at the top of the screen
    screen.pen = Pen(0xFF, 0xFF, 0xFF);
    screen.text("Score: 0", minimal_font, Point(Constants::SCREEN_WIDTH - 2, 2), true, TextAlign::top_right);
}