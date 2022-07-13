#pragma once

#include "32blit.hpp"

#include "utilities.hpp"
#include "constants.hpp"

using namespace blit;

class Ninja {
public:
	enum class Colour {
		BLUE = 0,
		RED = 1
	};

	Ninja();
	Ninja(Colour _colour, Vec2 _position);

	void update(float dt, Constants::LevelData& level_data);
	void render(Surface* screen);

	bool check_colliding(Vec2 object_position, uint8_t object_size);
	bool check_colliding(Ninja& ninja);

	Vec2 get_position();

protected:
	void handle_collisions(Constants::LevelData& level_data);
	void handle_platforms(Constants::LevelData& level_data, uint8_t x, uint8_t y);
	void handle_ladders(Constants::LevelData& level_data, uint8_t x, uint8_t y);

	// Only implemented by PlayerNinja
	virtual void handle_scoring(Constants::LevelData& level_data, uint8_t x, uint8_t y);

	void jump();

	Colour colour;

	Vec2 velocity, position;

	// Flags
	bool can_jump = false;
	bool can_climb = false;
	
	enum class ClimbingState {
		NONE,
		IDLE,
		UP,
		DOWN
	};

	ClimbingState climbing_state = ClimbingState::NONE;

	enum class HorizontalDirection {
		LEFT = -1,
		RIGHT = 1
	};
	enum class VerticalDirection {
		UP = -1,
		DOWN = 1
	};

	HorizontalDirection facing_direction = HorizontalDirection::RIGHT;

	// When a ninja is dead, they no longer collide with any tiles, but still act under gravity
	bool dead = false;
};