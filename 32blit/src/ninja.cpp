#include "ninja.hpp"

Ninja::Ninja() : _colour(Colour::Blue) {
	
}

Ninja::Ninja(Colour colour, float x, float y) : _colour(colour), position_x(x), position_y(y) {

}

void Ninja::update(float dt) {
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