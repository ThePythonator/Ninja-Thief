from player_ninja import PlayerNinja
from enemy_ninja import EnemyNinja
import constants as Constants

class Level:

    class LevelState:
        PLAYING = 0
        PLAYER_DEAD = 1
        PLAYER_WON = 2
        FAILED = 3
        COMPLETE = 4
    
    def __init__(self, level_number):
        self.level_number = level_number

        self.level_data = Constants.LEVELS[level_number].copy()

        self.level_state = Level.LevelState.PLAYING

        self.enemies = []

        # Search for player spawn position and create PlayerNinja object
        # Search for enemy spawn positions and create EnemyNinja objects and add them to a vector

        for y in range(Constants.GAME_HEIGHT_TILES):
            for x in range(Constants.GAME_WIDTH_TILES):
                spawn_id = self.level_data.entity_spawns[y * Constants.GAME_WIDTH_TILES + x]

                position_x = x * Constants.SPRITE_SIZE
                position_y = y * Constants.SPRITE_SIZE

                if spawn_id == Constants.Sprites.PLAYER_IDLE:
                    self.player = PlayerNinja(position_x, position_y)

                elif spawn_id == Constants.Sprites.PLAYER_IDLE + Constants.Sprites.PLAYER_OFFSET:
                    self.enemies.append(EnemyNinja(position_x, position_y))


    def update(self, dt):
        if self.level_state == Level.LevelState.PLAYING:
            # Update player
            self.player.update(dt, self.level_data)

            if self.coins_left() == 0:
                # No more coins left, so the player has won!
                self.level_state = Level.LevelState.PLAYER_WON

                self.player.set_won()

            # Update enemies
            for enemy in self.enemies:
                enemy.update(dt, self.level_data)

                if self.player.check_ninja_colliding(enemy):
                    # Player is now dead, trigger "fall" animation, before restarting level
                    self.level_state = Level.LevelState.PLAYER_DEAD

                    self.player.set_dead()
            

            if self.player.get_y() > Constants.GAME_HEIGHT:
                # Player has gone off the bottom of the screen, so they're dead
                self.level_state = Level.LevelState.FAILED
            

        elif self.level_state == Level.LevelState.PLAYER_DEAD:
            # Update player
            self.player.update(dt, self.level_data)

            if self.player.get_y() > Constants.GAME_HEIGHT:
                # Player has gone off the bottom of the screen
                self.level_state = Level.LevelState.FAILED


        elif self.level_state == Level.LevelState.PLAYER_WON:
            # Update player
            self.player.update(dt, self.level_data)

            if self.player.finished_celebrating() or self.player.get_y() > Constants.GAME_HEIGHT:
                # Player has finished doing victory jumps, or has fallen off the screen
                self.level_state = Level.LevelState.COMPLETE


    def render(self):
        # Render background pipes
        alpha(0x8)
        self.render_tiles(self.level_data.pipes)
        alpha()

        # Render platforms
        self.render_tiles(self.level_data.platforms)

        # Render extras (coins, gems and ladders)
        self.render_tiles(self.level_data.extras)

        # Render enemies
        for enemy in self.enemies:
            enemy.render()
        
        # Render player
        self.player.render()

        # Render UI text
        pen(0xf, 0xf, 0xf)

        # Render level number
        level_string = "Level: " + str(self.level_number + 1)
        text(level_string, 2, 2)

        # Render score
        score_string = "Score: " + str(self.player.get_score())

        # Get size of text
        w, h = measure(score_string)

        # Render text in top right corner
        text(score_string, Constants.SCREEN_WIDTH - 2 - w, 2)


    def level_failed(self):
        return self.level_state == Level.LevelState.FAILED


    def level_complete(self):
        return self.level_state == Level.LevelState.COMPLETE


    def render_tiles(self, tile_ids):
        # Iterate through array of tile ids and render using the correct index in the spritesheet
        for y in range(Constants.GAME_HEIGHT_TILES):
            for x in range(Constants.GAME_WIDTH_TILES):
                tile_id = tile_ids[y * Constants.GAME_WIDTH_TILES + x]

                if tile_id != Constants.Sprites.BLANK_TILE:
                    # We need to offset the tiles since the 32blit version has borders on the screen
                    sprite(tile_id, x * Constants.SPRITE_SIZE + Constants.GAME_OFFSET_X, y * Constants.SPRITE_SIZE + Constants.GAME_OFFSET_Y)


    def coins_left(self):
        total = 0

        for i in range(Constants.GAME_WIDTH_TILES * Constants.GAME_HEIGHT_TILES):
            if self.level_data.extras[i] == Constants.Sprites.COIN:
                total += 1

        return total
