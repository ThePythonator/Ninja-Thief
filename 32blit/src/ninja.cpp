#include "ninja.hpp"

Ninja::Ninja() : _colour(Colour::Blue) {
	
}

Ninja::Ninja(Colour colour, float x, float y) : _colour(colour), position_x(x), position_y(y) {

}

void Ninja::update(float dt, const uint8_t* level_data) {
	// Set can_jump to false - it is set to true later in this method, only if the player is on a platform
	can_jump = false;

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
					// Resolve collision by finding the direction with the least intersection
					// The value of direction is 0 if left side of tile, 1 if top, 2 if right, 3 if bottom
					uint8_t direction = 0;
					float least_intersection = Constants::SPRITE_SIZE;

					// Left side of tile
					float intersection = position_x + Constants::SPRITE_SIZE - Constants::NINJA_BORDER - tile_x;
					if (intersection < least_intersection) {
						direction = 0;
						least_intersection = intersection;
					}
					
					// Top side of tile
					intersection = position_y + Constants::SPRITE_SIZE - tile_y;
					if (intersection < least_intersection) {
						direction = 1;
						least_intersection = intersection;
					}

					// Right side of tile
					intersection = tile_x + Constants::SPRITE_SIZE - position_x - Constants::NINJA_BORDER;
					if (intersection < least_intersection) {
						direction = 2;
						least_intersection = intersection;
					}

					// Bottom side of tile
					intersection = tile_y + Constants::SPRITE_SIZE - position_y;
					if (intersection < least_intersection) {
						direction = 3;
						least_intersection = intersection;
					}

					// Now resolve collision by moving the player in the direction of least intersection, by exactly the amount equal to the least intersection
					switch (direction) {
					case 0:
						// Hit the left side of a platform
						position_x -= least_intersection;
						velocity_x = 0.0f;
						break;

					case 1:
						// Landed on top of a platform
						position_y -= least_intersection;
						velocity_y = 0.0f;

						// Allow the player to jump again
						can_jump = true;
						break;

					case 2:
						// Hit the right side of a platform
						position_x += least_intersection;
						velocity_x = 0.0f;
						break;

					case 3:
						// Hit the underside of a platform
						position_y += least_intersection;
						velocity_y = 0.0f;
						break;

					default:
						break;
					}
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