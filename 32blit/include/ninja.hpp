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

	void update(float dt, const Constants::LevelData& level_data);
	void render(Surface* screen);

	bool check_colliding(Vec2 object_position, uint8_t object_size);

protected:
	void handle_collisions(const Constants::LevelData& level_data);
	void handle_platforms(const Constants::LevelData& level_data, uint8_t x, uint8_t y);
	void handle_ladders(const Constants::LevelData& level_data, uint8_t x, uint8_t y);

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
};