from level import Level
import constants as Constants

from time import time

# Normally, global variables should be avoided where possible, but for simpicity they will sometimes be used in this project

# Setup the game

current_level = 0

# Used to calculate time since last frame
last_time = 0

# Set blend mode
blend(ALPHA)

# Load the spritesheet
sprites = Buffer(Constants.SPRITESHEET_WIDTH, Constants.SPRITESHEET_HEIGHT)
open("spritesheet.16bpp", "rb").readinto(sprites)

spritesheet(sprites)

# Load the background
background = Buffer(Constants.SCREEN_WIDTH, Constants.SCREEN_HEIGHT)
open("background.16bpp", "rb").readinto(background)

# Load the first level
level = Level(0)


# Update the game
def update(tick):
    global last_time, level, current_level

    # Calculate change in time (in seconds) since last frame
    dt = (time() - last_time)
    last_time = time()

    # Cap dt (in case the game is paused/lagging)
    if dt > 0.05:
        dt = 0.05

    level.update(dt)

    if level.level_failed():
        # Restart the same level
        level = Level(current_level)
    
    elif level.level_complete():
        # Start the next level
        current_level += 1
        current_level %= Constants.LEVEL_COUNT

        level = Level(current_level)


# Render the game (draw it to the display)
def draw(tick):
    # Clear the screen
    pen(0, 0, 0)
    clear()

    # Draw the background
    blit(background, 0, 0, Constants.SCREEN_WIDTH, Constants.SCREEN_HEIGHT, 0, 0)

    # Render the level, which includes drawing the player and enemies
    level.render()


# Start the game
start()