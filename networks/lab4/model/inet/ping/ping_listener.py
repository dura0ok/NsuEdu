from abc import ABC, abstractmethod


class PingListener(ABC):
    @abstractmethod
    def disconnect_player(self, player_id: int) -> None:
        pass

    @abstractmethod
    def send_ping(self, player_id: int) -> None:
        pass
