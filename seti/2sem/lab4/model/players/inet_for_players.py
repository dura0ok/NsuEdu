from abc import ABC, abstractmethod

from lib import GamePlayer, GameMessage


class InetForPlayers(ABC):
    @abstractmethod
    def send_message(self, player: GamePlayer, message: GameMessage) -> None:
        pass

    @abstractmethod
    def remove_player_from_ping(self, player_id: int) -> None:
        pass
