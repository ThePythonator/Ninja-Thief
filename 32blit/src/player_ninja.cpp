#include "player_ninja.hpp"

PlayerNinja::PlayerNinja() {

}

PlayerNinja::PlayerNinja(float x, float y) : Ninja(Colour::Blue, x, y) {

}

void PlayerNinja::update(float dt, const uint8_t* level_data) {
	// Handle any buttons the user has pressed
	// Note: "else if" isn't used, because otherwise one direction will be favoured when both buttons are pressed
	// Instead, we add/subtract the velocity, so if both are pressed, nothing happens

	// If nothing is pressed, the player shouldn't move
	velocity_x = 0.0f;

	if (buttons & Button::DPAD_LEFT) {
		velocity_x -= Constants::Player::MAX_SPEED;
	}
	if (buttons & Button::DPAD_RIGHT) {
		velocity_x += Constants::Player::MAX_SPEED;
	}

	// Handle jumping
	if (buttons & Button::A) {
		if (can_jump) {
			velocity_y = -Constants::Player::JUMP_SPEED;
			can_jump = false;
		}
	}

	// Call parent update method
	Ninja::update(dt, level_data);


	// Temporary addition to stop player falling off bottom of screen
	if (position_y > Constants::GAME_HEIGHT - Constants::SPRITE_SIZE) {
		position_y = Constants::GAME_HEIGHT - Constants::SPRITE_SIZE;
		velocity_y = 0.0f;
	}
}