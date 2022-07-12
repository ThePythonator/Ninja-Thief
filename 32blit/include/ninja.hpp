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

	void update(float dt);
	void render(Surface* screen);

protected:
	Colour _colour;

	float velocity_x = 0.0f;
	float velocity_y = 0.0f;

	float position_x = 0.0f;
	float position_y = 0.0f;
};