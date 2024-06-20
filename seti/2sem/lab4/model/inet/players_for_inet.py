from abc import ABC, abstractmethod

from lib import GamePlayer


class PlayersForInet(ABC):
    @abstractmethod
    def disconnect_player(self, player_id: int) -> None:
        pass

    @abstractmethod
    def get_game_player_by_id(self, player_id: int) -> GamePlayer:
        pass

    @abstractmethod
    def send_ping(self, player_id: int) -> None:
        pass
