#pragma once

#include "ninja.hpp"

class EnemyNinja : public Ninja {
public:
	EnemyNinja();
	EnemyNinja(Vec2 position);

	void update(float dt, const Constants::LevelData& level_data);

private:
	// Returns true if there is a platform tile which is one block below and just in front of the ninja
	// This is used to work out when the ninja reaches the end of a platform
	bool platform_ahead(const Constants::LevelData& level_data);

	// Returns true if there is a ladder tile above or below the player's centre
	bool ladder_above_or_below(const Constants::LevelData& level_data, VerticalDirection direction);

	int8_t current_direction = rand() % 2 ? 1 : -1;

	enum class AIState {
		PATROLLING,
		CLIMBING
	};

	AIState ai_state = AIState::PATROLLING;
};