#include "player_ninja.hpp"

PlayerNinja::PlayerNinja() {

}

PlayerNinja::PlayerNinja(Vec2 position) : Ninja(Colour::BLUE, position) {

}

void PlayerNinja::update(float dt, Constants::LevelData& level_data) {
	// If nothing is pressed, the player shouldn't move
	velocity.x = 0.0f;

	if (won) {
		// Jump in celebration!

		if (can_jump && celebration_jumps_remaining > 0) {
			jump();
			celebration_jumps_remaining--;
		}
	}
	else if (!dead) {
		// Handle any buttons the user has pressed
		// Note: "else if" isn't used, because otherwise one direction will be favoured when both buttons are pressed
		// Instead, we add/subtract the velocity, so if both are pressed, nothing happens

		if (buttons & Button::DPAD_LEFT) {
			velocity.x -= Constants::Player::MAX_SPEED;
		}
		if (buttons & Button::DPAD_RIGHT) {
			velocity.x += Constants::Player::MAX_SPEED;
		}

		// Handle climbing
		if (can_climb) {
			bool up = buttons & Button::DPAD_UP;
			bool down = buttons & Button::DPAD_DOWN;

			if (up != down) {
				// Only one of up and down are selected
				climbing_state = up ? ClimbingState::UP : ClimbingState::DOWN;
			}
			else if (climbing_state != ClimbingState::NONE) {
				// Player has already been climbing the ladder, and either none or both of up and down are pressed
				climbing_state = ClimbingState::IDLE;
			}
		}

		// Handle jumping
		if (buttons & Button::A) {
			if (can_jump) {
				jump();
			}
		}
	}

	// Call parent update method
	Ninja::update(dt, level_data);
}

void PlayerNinja::handle_scoring(Constants::LevelData& level_data, uint8_t x, uint8_t y) {
	// Calculate position of tile in array
	uint8_t array_position = y * Constants::GAME_WIDTH_TILES + x;

	// Get tile's sprite index from level data
	uint8_t tile_id = level_data.extras[array_position];

	// Check the tile is a coin or gem
	if (tile_id == Constants::Sprites::COIN || tile_id == Constants::Sprites::GEM) {
		Vec2 tile_position = Vec2(x, y) * Constants::SPRITE_SIZE;

		// Is the ninja colliding with the tile?
		// Note: we use a smaller object_size since the coins and gems are smaller, which also means we have to offset the tile_position
		if (check_colliding(tile_position + Vec2(1.0f, 1.0f) * Constants::Collectable::BORDER, Constants::Collectable::SIZE)) {

			// Add the correct amount of score if it's a coin or gem tile
			if (tile_id == Constants::Sprites::COIN) {
				score += Constants::Collectable::COIN_SCORE;
			}
			else if (tile_id == Constants::Sprites::GEM) {
				score += Constants::Collectable::GEM_SCORE;
			}

			// Remove item from level data
			level_data.extras[array_position] = Constants::Sprites::BLANK_TILE;
		}
	}
}

uint8_t PlayerNinja::get_score() {
	return score;
}

bool PlayerNinja::finished_celebrating() {
	return can_jump && celebration_jumps_remaining == 0;
}

void PlayerNinja::set_dead() {
	dead = true;
	jump();
}

void PlayerNinja::set_won() {
	won = true;
}