#include "level.hpp"

Level::Level() {

}

Level::Level(const Constants::LevelData& level_data) : _level_data(level_data) {
	// Search for player spawn position and create PlayerNinja object
	// Search for enemy spawn positions and create EnemyNinja objects and add them to a vector

	for (uint8_t y = 0; y < Constants::GAME_HEIGHT_TILES; y++) {
		for (uint8_t x = 0; x < Constants::GAME_WIDTH_TILES; x++) {
			uint8_t spawn_id = _level_data.entity_spawns[y * Constants::GAME_WIDTH_TILES + x];

			Vec2 position = Vec2(x, y) * Constants::SPRITE_SIZE;

			switch (spawn_id) {
			case Constants::Sprites::PLAYER_IDLE:
				player = PlayerNinja(position);
				break;

			case Constants::Sprites::PLAYER_IDLE + Constants::Sprites::PLAYER_OFFSET:
				enemies.push_back(EnemyNinja(position));
				break;

			default:
				break;
			}
		}
	}
}

void Level::update(float dt) {
	switch (level_state) {
	case LevelState::PLAYING:

		// Update player
		player.update(dt, _level_data);

		if (coins_left() == 0) {
			// No more coins left, so the player has won!
			level_state = LevelState::PLAYER_WON;

			player.set_won();
		}


		// Update enemies
		for (EnemyNinja& enemy : enemies) {
			enemy.update(dt, _level_data);

			if (player.check_colliding(enemy)) {
				// Player is now dead, trigger "fall" animation, before restarting level
				level_state = LevelState::PLAYER_DEAD;

				player.set_dead();
			}
		}

		if (player.get_position().y > Constants::GAME_HEIGHT) {
			// Player has gone off the bottom of the screen, so they're dead
			level_state = LevelState::FAILED;
		}

		break;

	case LevelState::PLAYER_DEAD:

		// Update player
		player.update(dt, _level_data);

		if (player.get_position().y > Constants::GAME_HEIGHT) {
			// Player has gone off the bottom of the screen
			level_state = LevelState::FAILED;
		}

		break;

	case LevelState::PLAYER_WON:

		// Update player
		player.update(dt, _level_data);

		if (player.finished_celebrating()) {
			// Player has finished doing victory jumps
			level_state = LevelState::COMPLETE;
		}

		break;

	default:
		break;
	}
}

void Level::render(Surface* screen) {
	// Render border
	render_border(screen);

	// Render platforms
	render_tiles(screen, _level_data.platforms);

	// Render extras (coins, gems and ladders)
	render_tiles(screen, _level_data.extras);

	// Render enemies
	for (EnemyNinja& enemy : enemies) {
		enemy.render(screen);
	}
	
	// Render player
	player.render(screen);

	// Render score
	std::string score_string = "Score: " + std::to_string(player.get_score());
	screen->pen = Pen(0xFF, 0xFF, 0xFF);
	screen->text(score_string, minimal_font, Point(Constants::SCREEN_WIDTH - 2, 2), true, TextAlign::top_right);
}

bool Level::level_failed() {
	return level_state == LevelState::FAILED;
}

bool Level::level_complete() {
	return level_state == LevelState::COMPLETE;
}

void Level::render_tiles(Surface* screen, const uint8_t* tile_ids) {
	// Iterate through array of tile ids and render using the correct index in the spritesheet
	for (uint8_t y = 0; y < Constants::GAME_HEIGHT_TILES; y++) {
		for (uint8_t x = 0; x < Constants::GAME_WIDTH_TILES; x++) {
			uint8_t tile_id = tile_ids[y * Constants::GAME_WIDTH_TILES + x];

			if (tile_id != Constants::Sprites::BLANK_TILE) {
				// We need to offset the tiles since the 32blit version has borders on the screen
				screen->sprite(tile_id, Point(x, y) * Constants::SPRITE_SIZE + Constants::GAME_OFFSET);
			}
		}
	}
}

void Level::render_border(Surface* screen) {
	// Render border (only needed for 32blit, with the wider screen)
	for (uint8_t y = 0; y < Constants::SCREEN_HEIGHT; y += Constants::SPRITE_SIZE) {
		// Left border:
		uint8_t x = 0;
		while (x < Constants::GAME_OFFSET.x - Constants::SPRITE_SIZE) {
			screen->sprite(Constants::Sprites::BORDER_FULL, Point(x, y));
			x += Constants::SPRITE_SIZE;
		}
		screen->sprite(Constants::Sprites::BORDER_LEFT, Point(x, y));

		// Right border:
		x = Constants::SCREEN_WIDTH;
		while (x > Constants::SCREEN_WIDTH - Constants::GAME_OFFSET.x) {
			screen->sprite(Constants::Sprites::BORDER_FULL, Point(x, y));
			x -= Constants::SPRITE_SIZE;
		}
		screen->sprite(Constants::Sprites::BORDER_RIGHT, Point(x, y));
	}
}

uint8_t Level::coins_left() {
	uint8_t total = 0;

	for (uint8_t i = 0; i < Constants::GAME_WIDTH_TILES * Constants::GAME_HEIGHT_TILES; i++) {
		if (_level_data.extras[i] == Constants::Sprites::COIN) {
			total++;
		}
	}

	return total;
}