#include "utilities.hpp"

uint8_t tile_at_position(const uint8_t* tile_array, Vec2 position) {
	// Apply positional correction to account for rounding
	position -= Vec2(0.5f, 0.0f);

	// Check that the position is within the game bounds (if it isn't, return an empty tile)
	if (position.x < 0.0f || position.x > Constants::GAME_WIDTH ||
		position.y < 0.0f || position.y > Constants::GAME_HEIGHT) {
		return Constants::Sprites::BLANK_TILE;
	}

	// Get grid position of tile
	uint8_t x = position.x / Constants::SPRITE_SIZE;
	uint8_t y = position.y / Constants::SPRITE_SIZE;

	// If we've not return yet, then it's safe to get the tile from the level data
	return tile_array[y * Constants::GAME_WIDTH_TILES + x];
}