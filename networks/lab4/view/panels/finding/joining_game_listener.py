from abc import ABC, abstractmethod


class JoiningGameListener(ABC):
    @abstractmethod
    def close_the_game(self):
        pass

    @abstractmethod
    def back_to_start_menu(self):
        pass

    @abstractmethod
    def joining_to_game(self, game_key):
        pass
