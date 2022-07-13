#include "enemy_ninja.hpp"

EnemyNinja::EnemyNinja() : Ninja() {

}

EnemyNinja::EnemyNinja(Vec2 position) : Ninja(Colour::RED, position) {

}

void EnemyNinja::update(float dt, const Constants::LevelData& level_data) {
	switch (ai_state) {
	case AIState::PATROLLING:
		if (!platform_ahead(level_data)) {
			current_direction = -current_direction;
		}

		velocity.x = Constants::Enemy::MAX_SPEED * current_direction;

		break;
		
	case AIState::CLIMBING:
		break;

	default:
		break;
	}

	Ninja::update(dt, level_data);
}

bool EnemyNinja::platform_ahead(const Constants::LevelData& level_data) {
	// Get a position which would be just in front of the ninja (and one tile below them)
	Vec2 point = Vec2(position.x + Constants::SPRITE_SIZE / 2 + current_direction * Constants::Enemy::PLATFORM_DETECTION_WIDTH / 2, position.y + Constants::SPRITE_SIZE);

	// Get tile at that position
	uint8_t tile_id = tile_at_position(level_data.platforms, point);

	// Return true if the tile is a platform (i.e. isn't an empty tile)
	return tile_id != Constants::Sprites::BLANK_TILE;
}

bool EnemyNinja::ladder_above_or_below(const Constants::LevelData& level_data, VerticalDirection direction) {
	// Get a position which would be one tile above the player
	Vec2 point = Vec2(position.x , position.y + Constants::SPRITE_SIZE * static_cast<int8_t>(direction));

	// Get tile at that position
	uint8_t tile_id = tile_at_position(level_data.platforms, point);

	// Return true if the tile is a ladder
	return tile_id == Constants::Sprites::LADDER;
}