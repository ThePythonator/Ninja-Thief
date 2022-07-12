#pragma once

#include "ninja.hpp"

class PlayerNinja : public Ninja {
public:
	PlayerNinja();
	PlayerNinja(Vec2 position);

	void update(float dt, const Constants::LevelData& level_data);

private:
	
};