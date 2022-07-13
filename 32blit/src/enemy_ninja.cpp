#include "enemy_ninja.hpp"

EnemyNinja::EnemyNinja() : Ninja() {

}

EnemyNinja::EnemyNinja(Vec2 position) : Ninja(Colour::RED, position) {

}

void EnemyNinja::update(float dt, Constants::LevelData& level_data) {
	switch (ai_state) {
	case AIState::PATROLLING:
		if (!platform_ahead(level_data)) {
			current_direction = -current_direction;
		}

		velocity.x = Constants::Enemy::MAX_SPEED * current_direction;

		if (can_climb && climb_next_ladder) {
			bool can_go_up = ladder_above_or_below(level_data, VerticalDirection::UP);
			bool can_go_down = ladder_above_or_below(level_data, VerticalDirection::DOWN);

			if (can_go_up && can_go_down) {
				climbing_state = std::rand() % 2 ? ClimbingState::UP : ClimbingState::DOWN;
			}
			else if (can_go_up) {
				climbing_state = ClimbingState::UP;
			}
			else if (can_go_down) {
				climbing_state = ClimbingState::DOWN;
			}

			if (climbing_state != ClimbingState::NONE) {
				// We've now decided to climb
				ai_state = AIState::CLIMBING;
			}
		}
		else if (!can_climb) {
			// Keep "re-rolling" while we can't climb

			// Decide if we should climb the next ladder we find
			climb_next_ladder = random_bool(Constants::Enemy::CLIMB_NEXT_LADDER_CHANCE);
		}

		break;

	case AIState::CLIMBING:

		break;

	default:
		break;
	}

	Ninja::update(dt, level_data);

	// If we're no longer in a climbing state, switch back to patrolling
	if (climbing_state == ClimbingState::NONE) {
		ai_state = AIState::PATROLLING;
	}
}

bool EnemyNinja::platform_ahead(Constants::LevelData& level_data) {
	// Get a position which would be just in front of the ninja (and one tile below them)
	Vec2 point = Vec2(position.x + Constants::SPRITE_SIZE / 2 + current_direction * Constants::Enemy::PLATFORM_DETECTION_WIDTH / 2, position.y + Constants::SPRITE_SIZE);

	// Get tile at that position
	uint8_t tile_id = tile_at_position(level_data.platforms, point);

	// Return true if the tile is a platform (i.e. isn't an empty tile)
	return tile_id != Constants::Sprites::BLANK_TILE;
}

bool EnemyNinja::ladder_above_or_below(Constants::LevelData& level_data, VerticalDirection direction) {
	// Get a position which would be one tile above/below the ninja
	Vec2 point = Vec2(position.x , position.y + Constants::SPRITE_SIZE * static_cast<int8_t>(direction));

	// Get tile at that position
	uint8_t tile_id = tile_at_position(level_data.extras, point);

	// Return true if the tile is a ladder
	return tile_id == Constants::Sprites::LADDER;
}