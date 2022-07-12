#pragma once

#include <vector>

#include "32blit.hpp"

#include "player_ninja.hpp"
#include "constants.hpp"

using namespace blit;

class Level {
public:
	Level();
	Level(const Constants::LevelData level_data);

	void update(float dt);
	void render(Surface* screen);

private:
	void render_tiles(Surface* screen, const uint8_t* tile_ids);
	void render_border(Surface* screen);

	Constants::LevelData _level_data;

	PlayerNinja player;
	//std::vector<EnemyNinja> enemies;
};