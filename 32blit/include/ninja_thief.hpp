#pragma once

#include <ctime>

#include "32blit.hpp"

#include "assets.hpp"

#include "level.hpp"
#include "constants.hpp"

// Required 32blit API methods
void init();
void update(uint32_t time);
void render(uint32_t time);