#include "level.hpp"

using namespace blit;

Level::Level() {

}

Level::Level(uint8_t _level_number) {
	level_number = _level_number;

	level_data = Constants::LEVELS[level_number];

	// Search for player spawn position and create PlayerNinja object
	// Search for enemy spawn positions and create EnemyNinja objects and add them to a vector

	for (uint8_t y = 0; y < Constants::GAME_HEIGHT_TILES; y++) {
		for (uint8_t x = 0; x < Constants::GAME_WIDTH_TILES; x++) {
			uint8_t spawn_id = level_data.entity_spawns[y * Constants::GAME_WIDTH_TILES + x];

			float position_x = x * Constants::SPRITE_SIZE;
			float position_y = y * Constants::SPRITE_SIZE;

			switch (spawn_id) {
			case Constants::Sprites::PLAYER_IDLE:
				player = PlayerNinja(position_x, position_y);
				break;

			case Constants::Sprites::PLAYER_IDLE + Constants::Sprites::PLAYER_OFFSET:
				enemies.push_back(EnemyNinja(position_x, position_y));
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
		player.update(dt, level_data);

		if (coins_left() == 0) {
			// No more coins left, so the player has won!
			level_state = LevelState::PLAYER_WON;

			player.set_won();
		}


		// Update enemies
		for (EnemyNinja& enemy : enemies) {
			enemy.update(dt, level_data);

			if (player.check_colliding(enemy)) {
				// Player is now dead, trigger "fall" animation, before restarting level
				level_state = LevelState::PLAYER_DEAD;

				player.set_dead();
			}
		}

		if (player.get_y() > Constants::GAME_HEIGHT) {
			// Player has gone off the bottom of the screen, so they're dead
			level_state = LevelState::FAILED;
		}

		break;

	case LevelState::PLAYER_DEAD:

		// Update player
		player.update(dt, level_data);

		if (player.get_y() > Constants::GAME_HEIGHT) {
			// Player has gone off the bottom of the screen
			level_state = LevelState::FAILED;
		}

		break;

	case LevelState::PLAYER_WON:

		// Update player
		player.update(dt, level_data);

		if (player.finished_celebrating() || player.get_y() > Constants::GAME_HEIGHT) {
			// Player has finished doing victory jumps, or has fallen off the screen
			level_state = LevelState::COMPLETE;
		}

		break;

	default:
		break;
	}
}

void Level::render() {
	// Render border
	render_border();

	// Render background pipes
	screen.alpha = 0x80;
	render_tiles(level_data.pipes);
	screen.alpha = 0xff;

	// Render water
	render_water();

	// Render platforms
	render_tiles(level_data.platforms);

	// Render extras (coins, gems and ladders)
	render_tiles(level_data.extras);

	// Render enemies
	for (EnemyNinja& enemy : enemies) {
		enemy.render();
	}
	
	// Render player
	player.render();

	// Render UI text
	screen.pen = Pen(0xff, 0xff, 0xff);

	// Render level number
	std::string level_string = "Level: " + std::to_string(level_number + 1);
	screen.text(level_string, minimal_font, Point(2, 2));

	// Render score
	std::string score_string = "Score: " + std::to_string(player.get_score());
	screen.text(score_string, minimal_font, Point(Constants::SCREEN_WIDTH - 2, 2), true, TextAlign::top_right);
}

bool Level::level_failed() {
	return level_state == LevelState::FAILED;
}

bool Level::level_complete() {
	return level_state == LevelState::COMPLETE;
}

void Level::render_tiles(const uint8_t* tile_ids) {
	// Iterate through array of tile ids and render using the correct index in the spritesheet
	for (uint8_t y = 0; y < Constants::GAME_HEIGHT_TILES; y++) {
		for (uint8_t x = 0; x < Constants::GAME_WIDTH_TILES; x++) {
			uint8_t tile_id = tile_ids[y * Constants::GAME_WIDTH_TILES + x];

			if (tile_id != Constants::Sprites::BLANK_TILE) {
				// We need to offset the tiles since the 32blit version has borders on the screen
				screen.sprite(tile_id, Point(x * Constants::SPRITE_SIZE + Constants::GAME_OFFSET_X, y * Constants::SPRITE_SIZE + Constants::GAME_OFFSET_Y));
			}
		}
	}
}

void Level::render_border() {
	// Render border (only needed for 32blit, with the wider screen)
	for (uint8_t y = 0; y < Constants::SCREEN_HEIGHT; y += Constants::SPRITE_SIZE) {
		// Left border:
		uint8_t x = 0;
		while (x < Constants::GAME_OFFSET_X - Constants::SPRITE_SIZE) {
			screen.sprite(Constants::Sprites::BORDER_FULL, Point(x, y));
			x += Constants::SPRITE_SIZE;
		}
		screen.sprite(Constants::Sprites::BORDER_LEFT, Point(x, y));

		// Right border:
		x = Constants::SCREEN_WIDTH;
		while (x > Constants::SCREEN_WIDTH - Constants::GAME_OFFSET_X) {
			screen.sprite(Constants::Sprites::BORDER_FULL, Point(x, y));
			x -= Constants::SPRITE_SIZE;
		}
		screen.sprite(Constants::Sprites::BORDER_RIGHT, Point(x, y));
	}
}

void Level::render_water() {
	for (uint8_t i = 0; i < Constants::GAME_WIDTH_TILES; i++) {
		screen.sprite(Constants::Sprites::WATER, Point(Constants::GAME_OFFSET_X + i * Constants::SPRITE_SIZE, Constants::GAME_OFFSET_Y + Constants::GAME_HEIGHT - Constants::SPRITE_SIZE));
	}
}

uint8_t Level::coins_left() {
	uint8_t total = 0;

	for (uint8_t i = 0; i < Constants::GAME_WIDTH_TILES * Constants::GAME_HEIGHT_TILES; i++) {
		if (level_data.extras[i] == Constants::Sprites::COIN) {
			total++;
		}
	}

	return total;
}