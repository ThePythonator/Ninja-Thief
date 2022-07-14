#pragma once

#include "picosystem.hpp"

#include "assets.hpp"

#include "level.hpp"
#include "constants.hpp"

using namespace picosystem;

// Required 32blit API methods
void init();
void update(uint32_t tick);
void draw(uint32_t tick);