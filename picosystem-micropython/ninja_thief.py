from time import ticks_ms, ticks_diff

from level import Level
import constants as Constants

# Normally, global variables should be avoided where possible, but for simplicity they will sometimes be used in this project

# Setup the game

blend(MASK)

# Set backlight to max
backlight(100)

# Load the spritesheet
sprites = Buffer(Constants.SPRITESHEET_WIDTH, Constants.SPRITESHEET_HEIGHT, "assets/spritesheet.16bpp")

spritesheet(sprites)

# Load the background
background = Buffer(Constants.SCREEN_WIDTH, Constants.SCREEN_HEIGHT, "assets/background.16bpp")

# Load the first level
level = Level(0)

current_level = 0

# Used to calculate time since last frame
last_time = 0


# Update the game
def update(tick):
    global last_time, level, current_level

    # Calculate change in time (in seconds) since last frame
    dt = ticks_diff(ticks_ms(), last_time) / 1000
    last_time = ticks_ms()

    # Cap dt (in case the game is paused/lagging)
    if dt > 0.04:
        dt = 0.04

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
