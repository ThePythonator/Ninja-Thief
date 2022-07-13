#pragma once

#include "ninja.hpp"

class PlayerNinja : public Ninja {
public:
	PlayerNinja();
	PlayerNinja(Vec2 position);

	void update(float dt, Constants::LevelData& level_data);

	void handle_scoring(Constants::LevelData& level_data, uint8_t x, uint8_t y);

	uint8_t get_score();

	void set_dead();

private:
	uint8_t score = 0;
};