#include "ninja.hpp"

Ninja::Ninja() : colour(Colour::BLUE) {
	
}

Ninja::Ninja(Colour _colour, float x, float y) : colour(_colour), position_x(x), position_y(y) {

}

void Ninja::update(float dt, Constants::LevelData& level_data) {
	// Set can_jump to false - it is set to true later in this method, only if the ninja is on a platform
	can_jump = false;

	// Apply gravity, only if ninja isn't climbing a ladder (or if ninja is dead)
	if (climbing_state == ClimbingState::NONE || dead) {
		velocity_y += Constants::Environment::GRAVITY_ACCELERATION * dt;
	}

	// Update position from velocity
	position_x += velocity_x * dt;
	position_y += velocity_y * dt;

	// Don't allow ninja to go off the sides
	if (position_x < -Constants::Ninja::BORDER) {
		position_x = -Constants::Ninja::BORDER;
	}
	else if (position_x > Constants::GAME_WIDTH - Constants::SPRITE_SIZE + Constants::Ninja::BORDER) {
		position_x = Constants::GAME_WIDTH - Constants::SPRITE_SIZE + Constants::Ninja::BORDER;
	}

	// Detect and resolve any collisions with platforms, ladders, coins etc
	if (!dead) {
		handle_collisions(level_data);
	}


	// Update direction the ninja is facing
	if (velocity_x < 0.0f) {
		facing_direction = HorizontalDirection::LEFT;
	}
	else if (velocity_x > 0.0f) {
		facing_direction = HorizontalDirection::RIGHT;
	}
}

void Ninja::render() {
	// Calculate offset from sprite indices provided in constants.hpp (depending on colour)
	uint8_t index_offset = static_cast<uint8_t>(colour) * Constants::Sprites::PLAYER_OFFSET;

	// If ninja is travelling left, flip the image horizontally
	SpriteTransform transform = facing_direction == HorizontalDirection::RIGHT ? SpriteTransform::NONE : SpriteTransform::HORIZONTAL;

	// Set sprite index
	uint8_t index = index_offset;
	
	if (climbing_state == ClimbingState::NONE) {
		// Player isn't climbing
		// TODO: handle walking/jumping animations
		// For now:
		index += Constants::Sprites::PLAYER_IDLE;
	}
	else {
		// TODO: handle climbing animation
		// For now:
		index += Constants::Sprites::PLAYER_CLIMBING_IDLE;
	}


	screen.sprite(index, Point(std::round(position_x) + Constants::GAME_OFFSET_X, std::round(position_y) + Constants::GAME_OFFSET_Y), transform);
}

bool Ninja::check_colliding(float object_x, float object_y, uint8_t object_size) {
	return (position_x + Constants::SPRITE_SIZE - Constants::Ninja::BORDER > object_x) &&
		(position_x + Constants::Ninja::BORDER < object_x + object_size) &&
		(position_y + Constants::SPRITE_SIZE > object_y) &&
		(position_y < object_y + object_size);
}

bool Ninja::check_colliding(Ninja& ninja) {
	float ninja_x = ninja.get_x();
	float ninja_y = ninja.get_y();

	return (position_x + Constants::SPRITE_SIZE - Constants::Ninja::BORDER > ninja_x + Constants::Ninja::BORDER) &&
		(position_x + Constants::Ninja::BORDER < ninja_x + Constants::SPRITE_SIZE - Constants::Ninja::BORDER) &&
		(position_y + Constants::SPRITE_SIZE > ninja_y) &&
		(position_y < ninja_y + Constants::SPRITE_SIZE);
}

float Ninja::get_x() {
	return position_x;
}

float Ninja::get_y() {
	return position_y;
}

void Ninja::handle_collisions(Constants::LevelData& level_data) {
	// Reset can_climb flag (which then gets set by handle_ladders if the ninja is near a ladder)
	can_climb = false;

	// Handle collision detection and resolution:
	// Get position of ninja in "grid" of tiles
	// Note: we're relying on converting to integers to trucate and hence round down
	uint8_t x = position_x / Constants::SPRITE_SIZE;
	uint8_t y = position_y / Constants::SPRITE_SIZE;

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

			// Handle scoring
			handle_scoring(level_data, new_x, new_y);
		}
	}

	// If ninja can no longer climb, reset their climbing state
	if (!can_climb) {
		climbing_state = ClimbingState::NONE;
	}

	// If player is on a ladder, they can jump
	if (climbing_state != ClimbingState::NONE) {
		can_jump = true;
	}
}

void Ninja::handle_platforms(Constants::LevelData& level_data, uint8_t x, uint8_t y) {
	// Get tile's sprite index from level data
	uint8_t tile_id = level_data.platforms[y * Constants::GAME_WIDTH_TILES + x];

	// Check the tile actually exists
	if (tile_id != Constants::Sprites::BLANK_TILE) {
		float tile_x = x * Constants::SPRITE_SIZE;
		float tile_y = y * Constants::SPRITE_SIZE;

		// Is the ninja colliding with the tile?
		if (check_colliding(tile_x, tile_y, Constants::SPRITE_SIZE)) {

			// Does this platform have a ladder in front of it?
			if (level_data.extras[y * Constants::GAME_WIDTH_TILES + x] == Constants::Sprites::LADDER) {
				if (climbing_state == ClimbingState::NONE) {
					// The ninja is not on a ladder

					if (velocity_y > 0.0f) {
						// The ninja can only collide with the platform if they are falling

						if (position_y + Constants::SPRITE_SIZE - tile_y < Constants::ONE_WAY_PLATFORM_TOLERANCE) {
							// Set the ninja's position so that it rests on top of the platform, and reset it's vertical velocity to zero
							position_y = tile_y - Constants::SPRITE_SIZE;
							velocity_y = 0.0f;

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
				float intersection = position_x + Constants::SPRITE_SIZE - Constants::Ninja::BORDER - tile_x;
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
				intersection = tile_x + Constants::SPRITE_SIZE - position_x - Constants::Ninja::BORDER;
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

				// Now resolve collision by moving the ninja in the direction of least intersection, by exactly the amount equal to the least intersection
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

					// Allow the ninja to jump again
					can_jump = true;

					// Stop the ninja from climbing
					climbing_state = ClimbingState::NONE;
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

void Ninja::handle_ladders(Constants::LevelData& level_data, uint8_t x, uint8_t y) {
	// Get tile's sprite index from level data
	uint8_t tile_id = level_data.extras[y * Constants::GAME_WIDTH_TILES + x];

	// Check if the tile is a ladder
	if (tile_id == Constants::Sprites::LADDER) {
		float tile_x = x * Constants::SPRITE_SIZE;
		float tile_y = y * Constants::SPRITE_SIZE;

		// Now check if ninja is colliding with the tile
		if (check_colliding(tile_x, tile_y, Constants::SPRITE_SIZE)) {
			
			// Check that ninja is sufficiently close to ladder:
			if (std::abs(tile_x - position_x) < Constants::Ninja::WIDTH / 2) {
				can_climb = true;

				// Check if ninja should be climbing or idling on ladder
				if (climbing_state != ClimbingState::NONE) {
					// Lock position to ladder
					position_x = tile_x;

					// Set velocity to 0
					velocity_x = 0.0f;
					velocity_y = 0.0f;

					// Get climbing speed, depending on whether ninja is the player or an enemy
					float climbing_speed = colour == Colour::BLUE ? Constants::Player::CLIMBING_SPEED : Constants::Enemy::CLIMBING_SPEED;

					// If player is actually climbing the ladder, set vertical velocity to be in the right direction
					if (climbing_state == ClimbingState::UP) {
						velocity_y = -climbing_speed;
					}
					else if (climbing_state == ClimbingState::DOWN) {
						velocity_y = climbing_speed;
					}
				}
			}
		}
	}
}

void Ninja::handle_scoring(Constants::LevelData& level_data, uint8_t x, uint8_t y) {
	// Only implemented by PlayerNinja
}

void Ninja::jump(float jump_speed) {
	velocity_y = -jump_speed;
	can_jump = false;

	// Reset climbing state when player jumps
	climbing_state = ClimbingState::NONE;
}