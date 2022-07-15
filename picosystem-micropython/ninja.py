import constants as Constants

class Ninja:

    # We're using classes as enums
    class ClimbingState:
        NONE = 0
        IDLE = 1
        UP = 2
        DOWN = 3

    class HorizontalDirection:
        LEFT = 0
        RIGHT = 1

    class VerticalDirection:
        UP = 2
        DOWN = 3

    class Colour:
        BLUE = 0
        RED = 1


    def __init__(self, colour, x, y):
        self.colour = colour

        self.position_x = x
        self.position_y = y

        self.velocity_x = 0
        self.velocity_y = 0

        self.can_jump = False
        self.can_climb = False

        self.climbing_state = Ninja.ClimbingState.NONE

        self.facing_direction = Ninja.HorizontalDirection.RIGHT

        self.dead = False


    def update(self, dt, level_data):
        # Set can_jump to False - it is set to true later in this method, only if the ninja is on a platform
        self.can_jump = False

        # Apply gravity, only if ninja isn't climbing a ladder (or if ninja is dead)
        if self.climbing_state == Ninja.ClimbingState.NONE or self.dead:
            self.velocity_y += Constants.Environment.GRAVITY_ACCELERATION * dt

        # Update position from velocity
        self.position_x += self.velocity_x * dt
        self.position_y += self.velocity_y * dt

        # Don't allow ninja to go off the sides
        if self.position_x < -Constants.Ninja.BORDER:
            self.position_x = -Constants.Ninja.BORDER
        
        elif self.position_x > Constants.GAME_WIDTH - Constants.SPRITE_SIZE + Constants.Ninja.BORDER:
            self.position_x = Constants.GAME_WIDTH - Constants.SPRITE_SIZE + Constants.Ninja.BORDER
        

        # Detect and resolve any collisions with platforms, ladders, coins etc
        if not self.dead:
            self.handle_collisions(level_data)

        # Update direction the ninja is facing
        if self.velocity_x < 0:
            self.facing_direction = Ninja.HorizontalDirection.LEFT
        
        elif self.velocity_x > 0:
            self.facing_direction = Ninja.HorizontalDirection.RIGHT


    def render(self):
        # Calculate offset from sprite indices provided in constants.hpp (depending on colour)
        index_offset = self.colour * Constants.Sprites.PLAYER_OFFSET

        # If ninja is travelling left, flip the image horizontally (set the transform flags)
        transform_flags = 0 if self.facing_direction == Ninja.HorizontalDirection.RIGHT else HFLIP

        # Set sprite index
        index = index_offset

        if self.climbing_state == Ninja.ClimbingState.NONE:
            # Player isn't climbing
            # TODO: handle walking/jumping animations
            # For now:
            index += Constants.Sprites.PLAYER_IDLE
        
        else:
            # TODO: handle climbing animation
            # For now:
            index += Constants.Sprites.PLAYER_CLIMBING_IDLE
        

        sprite(index, round(self.position_x) + Constants.GAME_OFFSET_X, round(self.position_y) + Constants.GAME_OFFSET_Y, 1, 1, Constants.SPRITE_SIZE, Constants.SPRITE_SIZE, transform_flags)
    

    def check_object_colliding(self, object_x, object_y, object_size):
        return self.position_x + Constants.SPRITE_SIZE - Constants.Ninja.BORDER > object_x and self.position_x + Constants.Ninja.BORDER < object_x + object_size and \
            self.position_y + Constants.SPRITE_SIZE > object_y and self.position_y < object_y + object_size
    

    def check_ninja_colliding(self, ninja):
        ninja_x = ninja.get_x()
        ninja_y = ninja.get_y()

        return self.position_x + Constants.SPRITE_SIZE - Constants.Ninja.BORDER > ninja_x + Constants.Ninja.BORDER and self.position_x + Constants.Ninja.BORDER < ninja_x + Constants.SPRITE_SIZE - Constants.Ninja.BORDER and \
            self.position_y + Constants.SPRITE_SIZE > ninja_y and self.position_y < ninja_y + Constants.SPRITE_SIZE

    
    def get_x(self):
        return self.position_x

    def get_y(self):
        return self.position_y
    

    def handle_collisions(self, level_data):
        # Reset can_climb flag (which then gets set by handle_ladders if the ninja is near a ladder)
        self.can_climb = False

        # Handle collision detection and resolution:
        # Get position of ninja in "grid" of tiles
        # Note: we're relying on converting to integers to trucate and hence round down
        x = int(self.position_x // Constants.SPRITE_SIZE)
        y = int(self.position_y // Constants.SPRITE_SIZE)

        # Check the four tiles which the ninja might be colliding with (the top left tile is marked by the x and y previously calculated)
        # Note: it's possible the ninja is near the edge of the screen and you could end up checking tiles which don't exist (off the side of the screen)
        # Note: to adef this issue, we use the ternary operator to vary the maximum x and y offsets.
        for y_offset in range(1 if y == Constants.GAME_HEIGHT_TILES else 2):
            for x_offset in range(1 if x == Constants.GAME_WIDTH_TILES else 2):
                new_x = x + x_offset
                new_y = y + y_offset

                # Handle platforms
                self.handle_platforms(level_data, new_x, new_y)

                # Handle ladders
                self.handle_ladders(level_data, new_x, new_y)

                # Handle scoring
                self.handle_scoring(level_data, new_x, new_y)

        # If ninja can no longer climb, reset their climbing state
        if not self.can_climb:
            self.climbing_state = Ninja.ClimbingState.NONE
        
        # If player is on a ladder, they can jump
        if self.climbing_state != Ninja.ClimbingState.NONE:
            self.can_jump = True


    def handle_platforms(self, level_data, x, y):
        # Get tile's sprite index from level data
        tile_id = level_data.platforms[y * Constants.GAME_WIDTH_TILES + x]

        # Check the tile actually exists
        if tile_id != Constants.Sprites.BLANK_TILE:
            tile_x = x * Constants.SPRITE_SIZE
            tile_y = y * Constants.SPRITE_SIZE

            # Is the ninja colliding with the tile?
            if self.check_object_colliding(tile_x, tile_y, Constants.SPRITE_SIZE):

                # Does this platform have a ladder in front of it?
                if level_data.extras[y * Constants.GAME_WIDTH_TILES + x] == Constants.Sprites.LADDER:
                    if self.climbing_state == Ninja.ClimbingState.NONE:
                        # The ninja is not on a ladder

                        if self.velocity_y > 0:
                            # The ninja can only collide with the platform if they are falling

                            if self.position_y + Constants.SPRITE_SIZE - tile_y < Constants.ONE_WAY_PLATFORM_TOLERANCE:
                                # Set the ninja's position so that it rests on top of the platform, and reset it's vertical velocity to zero
                                self.position_y = tile_y - Constants.SPRITE_SIZE
                                self.velocity_y = 0

                                # Allow the ninja to jump again
                                self.can_jump = True
                
                else:
                    # The platform doesn't have a ladder in front of it. Handle the collision resolution normally

                    # Resolve collision by finding the direction with the least intersection
                    # The value of direction is 0 if left side of tile, 1 if top, 2 if right, 3 if bottom
                    direction = 0
                    least_intersection = Constants.SPRITE_SIZE

                    # Left side of tile
                    intersection = self.position_x + Constants.SPRITE_SIZE - Constants.Ninja.BORDER - tile_x
                    if intersection < least_intersection:
                        direction = 0
                        least_intersection = intersection
                    

                    # Top side of tile
                    intersection = self.position_y + Constants.SPRITE_SIZE - tile_y
                    if intersection < least_intersection:
                        direction = 1
                        least_intersection = intersection
                    

                    # Right side of tile
                    intersection = tile_x + Constants.SPRITE_SIZE - self.position_x - Constants.Ninja.BORDER
                    if intersection < least_intersection:
                        direction = 2
                        least_intersection = intersection
                    

                    # Bottom side of tile
                    intersection = tile_y + Constants.SPRITE_SIZE - self.position_y
                    if intersection < least_intersection:
                        direction = 3
                        least_intersection = intersection
                    

                    # Now resolve collision by moving the ninja in the direction of least intersection, by exactly the amount equal to the least intersection
                    if direction == 0:
                        # Hit the left side of a platform
                        self.position_x -= least_intersection
                        self.velocity_x = 0

                    elif direction == 1:
                        # Landed on top of a platform
                        self.position_y -= least_intersection
                        self.velocity_y = 0

                        # Allow the ninja to jump again
                        self.can_jump = True

                        # Stop the ninja from climbing
                        self.climbing_state = Ninja.ClimbingState.NONE

                    elif direction ==  2:
                        # Hit the right side of a platform
                        self.position_x += least_intersection
                        self.velocity_x = 0

                    elif direction == 3:
                        # Hit the underside of a platform
                        self.position_y += least_intersection
                        self.velocity_y = 0


    def handle_ladders(self, level_data, x, y):
        # Get tile's sprite index from level data
        tile_id = level_data.extras[y * Constants.GAME_WIDTH_TILES + x]

        # Check if the tile is a ladder
        if tile_id == Constants.Sprites.LADDER:
            tile_x = x * Constants.SPRITE_SIZE
            tile_y = y * Constants.SPRITE_SIZE

            # Now check if ninja is colliding with the tile
            if self.check_object_colliding(tile_x, tile_y, Constants.SPRITE_SIZE):
                
                # Check that ninja is sufficiently close to ladder:
                if abs(tile_x - self.position_x) < Constants.Ninja.WIDTH / 2:
                    self.can_climb = True

                    # Check if ninja should be climbing or idling on ladder
                    if self.climbing_state != Ninja.ClimbingState.NONE:
                        # Lock position to ladder
                        self.position_x = tile_x

                        # Set velocity to 0
                        self.velocity_x = 0
                        self.velocity_y = 0

                        # Get climbing speed, depending on whether ninja is the player or an enemy
                        climbing_speed =  Constants.Player.CLIMBING_SPEED if self.colour == Ninja.Colour.BLUE else Constants.Enemy.CLIMBING_SPEED

                        # If player is actually climbing the ladder, set vertical velocity to be in the right direction
                        if self.climbing_state == Ninja.ClimbingState.UP:
                            self.velocity_y = -climbing_speed
                        
                        elif self.climbing_state == Ninja.ClimbingState.DOWN:
                            self.velocity_y = climbing_speed


    def handle_scoring(self, level_data, x, y):
        # Only implemented by PlayerNinja
        pass
    

    def jump(self, jump_speed):
        self.velocity_y = -jump_speed
        self.can_jump = False

        # Reset climbing state when player jumps
        self.climbing_state = Ninja.ClimbingState.NONE
    