import constants as Constants

from random import random

def tile_at_position(tile_array, x, y):
	# Apply positional correction to account for rounding
	x -= 0.5

	# Check that the position is within the game bounds (if it isn't, return an empty tile)
	if x < 0 or x > Constants.GAME_WIDTH or y < 0 or y > Constants.GAME_HEIGHT:
		return Constants.Sprites.BLANK_TILE

	# Get grid position of tile
	grid_x = int(x // Constants.SPRITE_SIZE)
	grid_y = int(y // Constants.SPRITE_SIZE)

	# If we've not return yet, then it's safe to get the tile from the level data
	return tile_array[grid_y * Constants.GAME_WIDTH_TILES + grid_x]

def random_bool(probability):
	return random() < probability
