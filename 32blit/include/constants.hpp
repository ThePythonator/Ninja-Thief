#pragma once

#include <cstdint>

namespace Constants {
	// Screen size in pixels
	const uint8_t SCREEN_WIDTH = 160;
	const uint8_t SCREEN_HEIGHT = 120;

	// Sprite indices to use for rendering
	namespace Sprites {
		const uint8_t PLAYER_IDLE = 24;
		const uint8_t PLAYER_SEMI_IDLE = 25;
		const uint8_t PLAYER_WALKING_1 = 26;
		const uint8_t PLAYER_WALKING_2 = 27;

		// Number of images for each player
		const uint8_t PLAYER_IMAGES = 4;
	}
}