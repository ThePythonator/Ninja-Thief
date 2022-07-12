#include "ninja.hpp"

Ninja::Ninja() : _colour(Colour::Blue) {
	
}

Ninja::Ninja(Colour colour, float x, float y) : _colour(colour), position_x(x), position_y(y) {

}

void Ninja::update(float dt, const uint8_t* level_data) {
	// Apply gravity
	velocity_y += Constants::Environment::GRAVITY_ACCELERATION * dt;

	// Update position from velocity
	position_x += velocity_x * dt;
	position_y += velocity_y * dt;

	// Don't allow ninja to go off the sides
	if (position_x < -Constants::NINJA_BORDER) {
		position_x = -Constants::NINJA_BORDER;
	}
	else if (position_x > Constants::GAME_WIDTH - Constants::SPRITE_SIZE + Constants::NINJA_BORDER) {
		position_x = Constants::GAME_WIDTH - Constants::SPRITE_SIZE + Constants::NINJA_BORDER;
	}

	// Handle collision detection and resolution:
	// Get position of ninja in "grid" of tiles
	// Note: we're relying on converting to integers to trucate and hence round down
	uint8_t x = position_x / Constants::SPRITE_SIZE;
	uint8_t y = position_y / Constants::SPRITE_SIZE;

	// Check the four tiles which the player might be colliding with (the top left tile is marked by the x and y previously calculated)
	// Note: it's possible the player is near the edge of the screen and you could end up checking tiles which don't exist (off the side of the screen)
	// Note: to avoid this issue, we use the ternary operator to vary the maximum x and y offsets.
	for (uint8_t y_offset = 0; y_offset < (y == Constants::GAME_HEIGHT_TILES - 1 ? 1 : 2); y_offset++) {
		for (uint8_t x_offset = 0; x_offset < (x == Constants::GAME_WIDTH_TILES - 1 ? 1 : 2); x_offset++) {
			// Get tile's sprite index from level data
			uint8_t tile_id = level_data[(y + y_offset) * Constants::GAME_WIDTH_TILES + (x + x_offset)];

			// Check the tile actually exists
			if (tile_id != Constants::NO_TILE) {
				uint8_t tile_x = (x + x_offset) * Constants::SPRITE_SIZE;
				uint8_t tile_y = (y + y_offset) * Constants::SPRITE_SIZE;

				// Now check if ninja is colliding with the tile
				if (check_colliding(tile_x, tile_y, Constants::SPRITE_SIZE)) {
					// TODO: sat resolution

					printf("tile: %u, %u (%u)\n", tile_x / 8, tile_y / 8, tile_id);
					printf("pos: %f, %f\n", position_x, position_y);
					//printf("collision %u\n", rand());
				}
			}
		}
	}
}

void Ninja::render(Surface* screen) {
	// Calculate offset from sprite indices provided in constants.hpp (depending on colour)
	uint8_t index_offset = static_cast<uint8_t>(_colour) * Constants::Sprites::PLAYER_IMAGES;

	// If player is travelling left, flip the image horizontally
	//SpriteTransform transform = velocity_x >= 0.0f ? SpriteTransform::NONE : SpriteTransform::HORIZONTAL;

	// TODO: Calculate sprite index based on animation frame

	uint8_t index = index_offset + Constants::Sprites::PLAYER_IDLE;

	screen->sprite(index, Point(position_x, position_y) + Constants::GAME_OFFSET);
}


bool Ninja::check_colliding(float object_x, float object_y, uint8_t object_size) {
	return (position_x + Constants::SPRITE_SIZE - Constants::NINJA_BORDER > object_x) &&
		(position_x + Constants::NINJA_BORDER < object_x + object_size) &&
		(position_y + Constants::SPRITE_SIZE > object_y) &&
		(position_y < object_y + object_size);
}