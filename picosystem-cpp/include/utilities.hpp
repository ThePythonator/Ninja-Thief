#pragma once

#include "constants.hpp"

// Finds the tile which is at the position provided
uint8_t tile_at_position(const uint8_t* tile_array, float x, float y);

// Returns a boolean, with chance of being true equal to probability supplied
bool random_bool(float probability);