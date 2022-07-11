#include "level.hpp"

Level::Level() {

}

Level::Level(const uint8_t level_data[]) : _level_data(level_data) {

}

void Level::update() {

}

void Level::render(Surface* screen) {
	for (uint8_t y = 0; y < Constants::GAME_HEIGHT_TILES; y++) {
		for (uint8_t x = 0; x < Constants::GAME_WIDTH_TILES; x++) {
			uint8_t tile_id = _level_data[y * Constants::GAME_WIDTH_TILES + x];

			if (tile_id != Constants::NO_TILE) {
				screen->sprite(tile_id, Point(x, y) * Constants::SPRITE_SIZE + Point(Constants::SCREEN_WIDTH - Constants::GAME_WIDTH, Constants::SCREEN_HEIGHT - Constants::GAME_HEIGHT) / 2);
			}
		}
	}
}