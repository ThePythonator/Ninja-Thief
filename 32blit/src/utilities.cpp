#include "utilities.hpp"

uint8_t tile_at_position(const uint8_t* tile_array, float x, float y) {
	// Apply positional correction to account for rounding
	x -= 0.5f;

	// Check that the position is within the game bounds (if it isn't, return an empty tile)
	if (x < 0.0f || x > Constants::GAME_WIDTH ||
		y < 0.0f || y > Constants::GAME_HEIGHT) {
		return Constants::Sprites::BLANK_TILE;
	}

	// Get grid position of tile
	uint8_t grid_x = x / Constants::SPRITE_SIZE;
	uint8_t grid_y = y / Constants::SPRITE_SIZE;

	// If we've not return yet, then it's safe to get the tile from the level data
	return tile_array[grid_y * Constants::GAME_WIDTH_TILES + grid_x];
}

bool random_bool(float probability) {
	return (std::rand() % 10001) / 10000.0f < probability;
}