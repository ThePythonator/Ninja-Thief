from ninja import Ninja
import constants as Constants

class PlayerNinja(Ninja):

    def __init__(self, x, y):
        super().__init__(Ninja.Colour.BLUE, x, y)

        self.score = 0

        self.won = False

        self.celebration_jumps_remaining = Constants.Player.CELEBRATION_JUMP_COUNT


    def update(self, dt, level_data):
        # If nothing is pressed, the player shouldn't move
        self.velocity_x = 0

        if self.won:
            # Jump in celebration!

            if self.can_jump and self.celebration_jumps_remaining > 0:
                self.jump(Constants.Player.CELEBRATION_JUMP_SPEED)
                self.celebration_jumps_remaining -= 1
        
        elif not self.dead:
            # Handle any buttons the user has pressed
            # Note: "elif" isn't used, because otherwise one direction will be favoured when both buttons are pressed
            # Instead, we add/subtract the velocity, so if both are pressed, nothing happens

            if button(LEFT):
                self.velocity_x -= Constants.Player.MAX_SPEED
            
            if button(RIGHT):
                self.velocity_x += Constants.Player.MAX_SPEED
            

            # Handle climbing
            if self.can_climb:
                up = button(UP)
                down = button(DOWN)

                if up != down:
                    # Only one of up and down are selected
                    self.climbing_state = ClimbingState.UP if up else ClimbingState.DOWN
                
                elif self.climbing_state != ClimbingState.NONE:
                    # Player has already been climbing the ladder, and either none or both of up and down are pressed
                    self.climbing_state = ClimbingState.IDLE


            # Handle jumping
            if pressed(A):
                if self.can_jump:
                    # Player is on platform so is allowed to jump
                    self.jump(Constants.Player.JUMP_SPEED if self.climbing_state == ClimbingState.NONE else Constants.Player.CLIMBING_JUMP_SPEED)
        

        # Call parent update method
        Ninja.update(dt, level_data)


    def handle_scoring(self, level_data, x, y):
        # Calculate position of tile in array
        array_position = y * Constants.GAME_WIDTH_TILES + x

        # Get tile's sprite index from level data
        tile_id = level_data.extras[array_position]

        # Check the tile is a coin or gem
        if tile_id == Constants.Sprites.COIN or tile_id == Constants.Sprites.GEM:
            tile_x = x * Constants.SPRITE_SIZE
            tile_y = y * Constants.SPRITE_SIZE

            # Is the ninja colliding with the tile?
            # Note: we use a smaller object_size since the coins and gems are smaller, which also means we have to offset the tile_position
            if self.check_colliding(tile_x + Constants.Collectable.BORDER, tile_y + Constants.Collectable.BORDER, Constants.Collectable.SIZE):

                # Add the correct amount of score if it's a coin or gem tile
                if tile_id == Constants.Sprites.COIN:
                    self.score += Constants.Collectable.COIN_SCORE
                
                elif tile_id == Constants.Sprites.GEM:
                    self.score += Constants.Collectable.GEM_SCORE

                # Remove item from level data
                level_data.extras[array_position] = Constants.Sprites.BLANK_TILE


    def get_score(self):
        return self.score


    def finished_celebrating(self):
        return self.can_jump and self.celebration_jumps_remaining == 0


    def set_dead(self):
        self.dead = true
        self.jump(Constants.Player.DEATH_JUMP_SPEED)


    def set_won(self):
        self.won = true
