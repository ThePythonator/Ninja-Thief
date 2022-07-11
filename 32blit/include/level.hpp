#pragma once

#include "32blit.hpp"

#include "constants.hpp"

using namespace blit;

class Level {
public:
	Level();
	Level(const uint8_t level_data[]);

	void update();
	void render(Surface* screen);

private:
	const uint8_t* _level_data;
};