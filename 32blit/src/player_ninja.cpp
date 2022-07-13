#include "player_ninja.hpp"

PlayerNinja::PlayerNinja() {

}

PlayerNinja::PlayerNinja(Vec2 position) : Ninja(Colour::BLUE, position) {

}

void PlayerNinja::update(float dt, const Constants::LevelData& level_data) {
	// Handle any buttons the user has pressed
	// Note: "else if" isn't used, because otherwise one direction will be favoured when both buttons are pressed
	// Instead, we add/subtract the velocity, so if both are pressed, nothing happens

	// If nothing is pressed, the player shouldn't move
	velocity.x = 0.0f;

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

	// Call parent update method
	Ninja::update(dt, level_data);
}