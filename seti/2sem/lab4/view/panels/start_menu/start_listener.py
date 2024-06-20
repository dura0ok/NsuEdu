from abc import ABC, abstractmethod


class StartListener(ABC):
    @abstractmethod
    def close_the_game(self):
        pass

    @abstractmethod
    def create_new_game(self):
        pass

    @abstractmethod
    def find_games(self):
        pass
