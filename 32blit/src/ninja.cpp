#include "ninja.hpp"

Ninja::Ninja() : colour(Colour::BLUE) {
	
}

Ninja::Ninja(Colour _colour, Vec2 _position) : colour(_colour), position(_position) {

}

void Ninja::update(float dt, const Constants::LevelData& level_data) {
	// Set can_jump to false - it is set to true later in this method, only if the ninja is on a platform
	can_jump = false;

	// Apply gravity, only if ninja isn't climbing a ladder
	if (climbing_state == ClimbingState::NONE) velocity.y += Constants::Environment::GRAVITY_ACCELERATION * dt;

	// Update position from velocity
	position += velocity * dt;

	// Don't allow ninja to go off the sides
	if (position.x < -Constants::NINJA_BORDER) {
		position.x = -Constants::NINJA_BORDER;
	}
	else if (position.x > Constants::GAME_WIDTH - Constants::SPRITE_SIZE + Constants::NINJA_BORDER) {
		position.x = Constants::GAME_WIDTH - Constants::SPRITE_SIZE + Constants::NINJA_BORDER;
	}

	// Detect and resolve any collisions with platforms, ladders, coins etc
	handle_collisions(level_data);


	// Update direction the ninja is facing
	if (velocity.x < 0.0f) {
		facing_direction = HorizontalDirection::LEFT;
	}
	else if (velocity.x > 0.0f) {
		facing_direction = HorizontalDirection::RIGHT;
	}
}

void Ninja::render(Surface* screen) {
	// Calculate offset from sprite indices provided in constants.hpp (depending on colour)
	uint8_t index_offset = static_cast<uint8_t>(colour) * Constants::Sprites::PLAYER_IMAGES;

	// If ninja is travelling left, flip the image horizontally
	SpriteTransform transform = facing_direction == HorizontalDirection::RIGHT ? SpriteTransform::NONE : SpriteTransform::HORIZONTAL;

	// TODO: Calculate sprite index based on animation frame

	/*// Set sprite index
	uint8_t index = index_offset;
	
	if (climbing_state == ClimbingState::NONE) {
		// Player isn't climbing

		if (velocity_x == 0.0f && velocity_y == 0.0f) {
			// Player isn't moving
			index += Constants::Sprites::PLAYER_IDLE;
		}
		else {
			// Player is moving
			//index += Constants::Sprites::PLAYER_WALKING;
		}
	}
	else {
		// TODO: handle climbing animation
		// For now:
		//index += 
	}*/

	uint8_t index = index_offset + Constants::Sprites::PLAYER_IDLE;


	screen->sprite(index, Point(position.x, position.y) + Constants::GAME_OFFSET, transform);
}

bool Ninja::check_colliding(Vec2 object_position, uint8_t object_size) {
	return (position.x + Constants::SPRITE_SIZE - Constants::NINJA_BORDER > object_position.x) &&
		(position.x + Constants::NINJA_BORDER < object_position.x + object_size) &&
		(position.y + Constants::SPRITE_SIZE > object_position.y) &&
		(position.y < object_position.y + object_size);
}

void Ninja::handle_collisions(const Constants::LevelData& level_data) {
	// Reset can_climb flag (which then gets set by handle_ladders if the ninja is near a ladder)
	can_climb = false;

	// Handle collision detection and resolution:
	// Get position of ninja in "grid" of tiles
	// Note: we're relying on converting to integers to trucate and hence round down
	uint8_t x = position.x / Constants::SPRITE_SIZE;
	uint8_t y = position.y / Constants::SPRITE_SIZE;

	// Check the four tiles which the ninja might be colliding with (the top left tile is marked by the x and y previously calculated)
	// Note: it's possible the ninja is near the edge of the screen and you could end up checking tiles which don't exist (off the side of the screen)
	// Note: to avoid this issue, we use the ternary operator to vary the maximum x and y offsets.
	for (uint8_t y_offset = 0; y_offset < (y == Constants::GAME_HEIGHT_TILES - 1 ? 1 : 2); y_offset++) {
		for (uint8_t x_offset = 0; x_offset < (x == Constants::GAME_WIDTH_TILES - 1 ? 1 : 2); x_offset++) {
			uint8_t new_x = x + x_offset;
			uint8_t new_y = y + y_offset;

			// Handle platforms
			handle_platforms(level_data, new_x, new_y);

			// Handle ladders
			handle_ladders(level_data, new_x, new_y);
		}
	}

	// If ninja can no longer climb, reset their climbing state
	if (!can_climb) climbing_state = ClimbingState::NONE;

	// If player is on a ladder, they can jump
	if (climbing_state != ClimbingState::NONE) can_jump = true;
}

void Ninja::handle_platforms(const Constants::LevelData& level_data, uint8_t x, uint8_t y) {
	// Get tile's sprite index from level data
	uint8_t tile_id = level_data.platforms[y * Constants::GAME_WIDTH_TILES + x];

	// Check the tile actually exists
	if (tile_id != Constants::Sprites::BLANK_TILE) {
		Vec2 tile_position = Vec2(x, y) * Constants::SPRITE_SIZE;

		// Is the ninja colliding with the tile?
		if (check_colliding(tile_position, Constants::SPRITE_SIZE)) {

			// Does this platform have a ladder in front of it?
			if (level_data.extras[y * Constants::GAME_WIDTH_TILES + x] == Constants::Sprites::LADDER) {
				if (climbing_state == ClimbingState::NONE) {
					// The ninja is not on a ladder

					if (velocity.y > 0.0f) {
						// The ninja can only collide with the platform if they are falling

						if (position.y + Constants::SPRITE_SIZE - tile_position.y < Constants::ONE_WAY_PLATFORM_TOLERANCE) {
							// Set the ninja's position so that it rests on top of the platform, and reset it's vertical velocity to zero
							position.y = tile_position.y - Constants::SPRITE_SIZE;
							velocity.y = 0.0f;

							// Allow the ninja to jump again
							can_jump = true;
						}
					}
				}
			}
			else {
				// The platform doesn't have a ladder in front of it. Handle the collision resolution normally

				// Resolve collision by finding the direction with the least intersection
				// The value of direction is 0 if left side of tile, 1 if top, 2 if right, 3 if bottom
				uint8_t direction = 0;
				float least_intersection = Constants::SPRITE_SIZE;

				// Left side of tile
				float intersection = position.x + Constants::SPRITE_SIZE - Constants::NINJA_BORDER - tile_position.x;
				if (intersection < least_intersection) {
					direction = 0;
					least_intersection = intersection;
				}

				// Top side of tile
				intersection = position.y + Constants::SPRITE_SIZE - tile_position.y;
				if (intersection < least_intersection) {
					direction = 1;
					least_intersection = intersection;
				}

				// Right side of tile
				intersection = tile_position.x + Constants::SPRITE_SIZE - position.x - Constants::NINJA_BORDER;
				if (intersection < least_intersection) {
					direction = 2;
					least_intersection = intersection;
				}

				// Bottom side of tile
				intersection = tile_position.y + Constants::SPRITE_SIZE - position.y;
				if (intersection < least_intersection) {
					direction = 3;
					least_intersection = intersection;
				}

				// Now resolve collision by moving the ninja in the direction of least intersection, by exactly the amount equal to the least intersection
				switch (direction) {
				case 0:
					// Hit the left side of a platform
					position.x -= least_intersection;
					velocity.x = 0.0f;
					break;

				case 1:
					// Landed on top of a platform
					position.y -= least_intersection;
					velocity.y = 0.0f;

					// Allow the ninja to jump again
					can_jump = true;

					// Stop the ninja from climbing
					climbing_state = ClimbingState::NONE;
					break;

				case 2:
					// Hit the right side of a platform
					position.x += least_intersection;
					velocity.x = 0.0f;
					break;

				case 3:
					// Hit the underside of a platform
					position.y += least_intersection;
					velocity.y = 0.0f;
					break;

				default:
					break;
				}
			}
		}
	}
}

void Ninja::handle_ladders(const Constants::LevelData& level_data, uint8_t x, uint8_t y) {
	// Get tile's sprite index from level data
	uint8_t tile_id = level_data.extras[y * Constants::GAME_WIDTH_TILES + x];

	// Check if the tile is a ladder
	if (tile_id == Constants::Sprites::LADDER) {
		Vec2 tile_position = Vec2(x, y) * Constants::SPRITE_SIZE;

		// Now check if ninja is colliding with the tile
		if (check_colliding(tile_position, Constants::SPRITE_SIZE)) {
			
			// Check that ninja is sufficiently close to ladder:
			if (std::abs(tile_position.x - position.x) < Constants::NINJA_WIDTH / 2) {
				can_climb = true;

				// Check if ninja should be climbing or idling on ladder
				if (climbing_state != ClimbingState::NONE) {
					// Lock position to ladder
					position.x = tile_position.x;

					// Set velocity to 0
					velocity = Vec2();

					// If player is actually climbing the ladder, set vertical velocity to be in the right direction
					if (climbing_state == ClimbingState::UP) {
						velocity.y = -Constants::Player::CLIMBING_SPEED;
					}
					else if (climbing_state == ClimbingState::DOWN) {
						velocity.y = Constants::Player::CLIMBING_SPEED;
					}
				}
			}
		}
	}
}

void Ninja::jump() {
	// Player is on platform so is allowed to jump
	velocity.y = -Constants::Player::JUMP_SPEED;
	can_jump = false;

	// Reset climbing state when player jumps
	climbing_state = ClimbingState::NONE;
}