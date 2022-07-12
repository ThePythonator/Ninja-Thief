#pragma once

#include <vector>

#include "32blit.hpp"

#include "player_ninja.hpp"
#include "constants.hpp"

using namespace blit;

class Level {
public:
	Level();
	Level(const uint8_t level_data[]);

	void update(float dt);
	void render(Surface* screen);

private:
	const uint8_t* _level_data = nullptr;

	PlayerNinja player;
	//std::vector<EnemyNinja> enemies;
};