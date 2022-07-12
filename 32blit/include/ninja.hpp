#pragma once

#include "32blit.hpp"

#include "constants.hpp"

using namespace blit;

class Ninja {
public:
	enum class Colour {
		Blue = 0,
		Red = 1
	};

	Ninja();
	Ninja(Colour colour, float x, float y);

	void update(float dt, const uint8_t* level_data);
	void render(Surface* screen);

	bool check_colliding(float object_x, float object_y, uint8_t object_size);

protected:
	Colour _colour;

	float velocity_x = 0.0f;
	float velocity_y = 0.0f;

	float position_x = 0.0f;
	float position_y = 0.0f;

	bool can_jump = false;
};