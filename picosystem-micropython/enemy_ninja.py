from random import choice

from ninja import Ninja
import utilities
import constants as Constants

class EnemyNinja(Ninja):

    class AIState:
        PATROLLING = 0
        CLIMBING = 1

    def __init__(self, x, y):
        super().__init__(Ninja.Colour.RED, x, y)

        self.current_direction = choice([-1, 1])
        self.climb_next_ladder = choice([True, False])

        self.ai_state = EnemyNinja.AIState.PATROLLING


    def update(self, dt, level_data):
        if self.ai_state == EnemyNinja.AIState.PATROLLING:
            if not self.platform_ahead(level_data):
                self.current_direction = -self.current_direction

            self.velocity_x = Constants.Enemy.MAX_SPEED * self.current_direction

            if self.can_climb and self.climb_next_ladder:
                can_go_up = self.ladder_above_or_below(level_data, VerticalDirection.UP)
                can_go_down = self.ladder_above_or_below(level_data, VerticalDirection.DOWN)

                if can_go_up and can_go_down:
                    self.climbing_state = choice([Ninja.ClimbingState.UP, Ninja.ClimbingState.DOWN])
                
                elif can_go_up:
                    self.climbing_state = Ninja.ClimbingState.UP
                
                elif can_go_down:
                    self.climbing_state = Ninja.ClimbingState.DOWN
                

                if self.climbing_state != Ninja.ClimbingState.NONE:
                    # We've now decided to climb
                    self.ai_state = EnemyNinja.AIState.CLIMBING
                
            
            elif not self.can_climb:
                # Keep "re-rolling" while we can't climb

                # Decide if we should climb the next ladder we find
                self.climb_next_ladder = random_bool(Constants.Enemy.CLIMB_NEXT_LADDER_CHANCE)


        Ninja.update(dt, level_data)

        # If we're no longer in a climbing state, switch back to patrolling
        if self.climbing_state == Ninja.ClimbingState.NONE:
            self.ai_state = EnemyNinja.AIState.PATROLLING


    def platform_ahead(self, level_data):
        # Get a position which would be just in front of the ninja (and one tile below them)
        point_x = self.position_x + Constants.SPRITE_SIZE / 2 + self.current_direction * Constants.Enemy.PLATFORM_DETECTION_WIDTH / 2
        point_y = self.position_y + Constants.SPRITE_SIZE

        # Get tile at that position
        tile_id = tile_at_position(level_data.platforms, point_x, point_y)

        # Return true if the tile is a platform (i.e. isn't an empty tile)
        return tile_id != Constants.Sprites.BLANK_TILE


    def ladder_above_or_below(self, level_data, direction):
        # Get a position which would be one tile above/below the ninja
        point_x = self.position_x
        point_y = self.position_y + Constants.SPRITE_SIZE * direction

        # Get tile at that position
        tile_id = tile_at_position(level_data.extras, point_x, point_y)

        # Return true if the tile is a ladder
        return tile_id == Constants.Sprites.LADDER
