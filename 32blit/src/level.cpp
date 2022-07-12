#include "level.hpp"

Level::Level() {

}

Level::Level(const Constants::LevelData level_data) : _level_data(level_data) {
	// TODO: search for player pos and create PlayerNinja object

	// For now, just spawn player in centre of screen
	player = PlayerNinja(Constants::GAME_WIDTH / 2, Constants::GAME_HEIGHT / 2);
}

void Level::update(float dt) {
	player.update(dt, _level_data);
}

void Level::render(Surface* screen) {
	// Render border
	render_border(screen);

	// Render platforms
	render_tiles(screen, _level_data.platforms);

	// Render extras (coins, gems and ladders)
	render_tiles(screen, _level_data.extras);
	
	// Render player
	player.render(screen);
}

void Level::render_tiles(Surface* screen, const uint8_t* tile_ids) {
	// Iterate through array of tile ids and render using the correct index in the spritesheet
	for (uint8_t y = 0; y < Constants::GAME_HEIGHT_TILES; y++) {
		for (uint8_t x = 0; x < Constants::GAME_WIDTH_TILES; x++) {
			uint8_t tile_id = tile_ids[y * Constants::GAME_WIDTH_TILES + x];

			if (tile_id != Constants::NO_TILE) {
				// We need to offset the tiles since the 32blit version has borders on the screen
				screen->sprite(tile_id, Point(x, y) * Constants::SPRITE_SIZE + Constants::GAME_OFFSET);
			}
		}
	}
}

void Level::render_border(Surface* screen) {
	// Render border (only needed for 32blit, with the wider screen)
	for (uint8_t y = 0; y < Constants::SCREEN_HEIGHT; y += Constants::SPRITE_SIZE) {
		// Left border:
		uint8_t x = 0;
		while (x < Constants::GAME_OFFSET.x - Constants::SPRITE_SIZE) {
			screen->sprite(Constants::Sprites::BORDER_FULL, Point(x, y));
			x += Constants::SPRITE_SIZE;
		}
		screen->sprite(Constants::Sprites::BORDER_LEFT, Point(x, y));

		// Right border:
		x = Constants::SCREEN_WIDTH;
		while (x > Constants::SCREEN_WIDTH - Constants::GAME_OFFSET.x) {
			screen->sprite(Constants::Sprites::BORDER_FULL, Point(x, y));
			x -= Constants::SPRITE_SIZE;
		}
		screen->sprite(Constants::Sprites::BORDER_RIGHT, Point(x, y));
	}
}