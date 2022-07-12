#pragma once

#include "ninja.hpp"

class PlayerNinja : public Ninja {
public:
	PlayerNinja();
	PlayerNinja(float x, float y);

	void update(float dt, const Constants::LevelData level_data);

private:
	
};