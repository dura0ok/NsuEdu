import time
from threading import Thread
import traceback

from lib import Direction
from model.action_updater.game_core_for_action_updater import GameCoreForActionUpdater
from model.action_updater.players_for_action_updater import PlayersForActionUpdater


class ActionUpdater(Thread):
    def __init__(self, game_core, players, state_delay: int):
        super().__init__()
        self.game_core = game_core
        self.players = players
        self.state_delay = state_delay
        self.is_alive = True
        self.players_actions: dict[int, Direction] = {}

    def set_change_master_breakpoint(self) -> None:
        print("Action Updater: exit breakpoint was set")
        self.is_alive = False

    def run(self) -> None:
        try:
            time.sleep(self.state_delay / 1000 / 4)
            print('updater started')
            while self.is_alive:
                self.make_actions()
                self.players.initiate_deputy_player()
                self.players.send_game_state_to_all_players()
                time.sleep(self.state_delay / 1000)
        except Exception as e:
            print(f"Action Updater: {traceback.format_exc()}")
        finally:
            print("Action Updater finished")
            self.players.transfer_role_of_the_master()

    def add_action(self, player_id: int, direction: Direction) -> None:
        self.players_actions[player_id] = direction

    def add_snakes_to_action_updater(self) -> None:
        for player_id in self.game_core.get_list_of_snakes_id():
            self.add_new_player(player_id)

    def add_new_player(self, new_player_id: int) -> None:
        player_snake_direction = self.game_core.get_snake_direction(new_player_id)
        self.players_actions[new_player_id] = player_snake_direction

    def remove_player(self, player_id: int) -> None:
        if player_id in self.players_actions:
            del self.players_actions[player_id]

    def make_actions(self) -> None:
        for player_id in list(self.players_actions.keys()):
            self.game_core.make_action(player_id, self.players_actions[player_id])
        self.game_core.update_field()
