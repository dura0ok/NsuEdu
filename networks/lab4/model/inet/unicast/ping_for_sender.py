from abc import ABC, abstractmethod


class PingForSender(ABC):
    @abstractmethod
    def is_alive_player(self, player_id: int):
        pass

    @abstractmethod
    def set_time_of_sent_message(self, player_id: int):
        pass
