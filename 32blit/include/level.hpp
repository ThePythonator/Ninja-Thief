#pragma once

#include <vector>
#include <string>

#include "32blit.hpp"

#include "player_ninja.hpp"
#include "enemy_ninja.hpp"
#include "constants.hpp"

using namespace blit;

class Level {
public:
	Level();
	Level(uint8_t _level_number);

	void update(float dt);
	void render(Surface* screen);

	bool level_failed();
	bool level_complete();

private:
	void render_tiles(Surface* screen, const uint8_t* tile_ids);
	void render_border(Surface* screen);

	uint8_t coins_left();

	Constants::LevelData level_data = {};
	uint8_t level_number = 0;

	PlayerNinja player;
	std::vector<EnemyNinja> enemies;

	enum class LevelState {
		PLAYING,
		PLAYER_DEAD,
		PLAYER_WON,
		FAILED,
		COMPLETE
	};

	LevelState level_state = LevelState::PLAYING;
};