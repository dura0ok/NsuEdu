from abc import ABC, abstractmethod

from lib import NodeRole, GameState


class PlayersListener(ABC):
    @abstractmethod
    def add_player_in_game(self, new_player: int) -> bool:
        pass

    @abstractmethod
    def get_game_state_data(self) -> GameState:
        pass

    @abstractmethod
    def change_this_node_role(self, role: NodeRole, request_from_player: bool) -> None:
        pass
