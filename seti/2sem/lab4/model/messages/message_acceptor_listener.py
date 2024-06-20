from abc import ABC, abstractmethod


class MessageAcceptorListener(ABC):
    @abstractmethod
    def launch_game_core(self, player_id):
        pass

    @abstractmethod
    def get_game_player_by_id(self, player_id):
        pass

    @abstractmethod
    def send_ack_message(self, player, message):
        pass

    @abstractmethod
    def set_time_of_received_message(self, player_id):
        pass
