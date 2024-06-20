from abc import abstractmethod, ABC

from lib import Direction


class GameCoreForActionUpdater(ABC):
    @abstractmethod
    def update_field(self):
        pass

    @abstractmethod
    def make_action(self, player_id: int, direction: Direction):
        pass

    @abstractmethod
    def get_snake_direction(self, player_id: int) -> Direction:
        pass

    @abstractmethod
    def get_list_of_snakes_id(self) -> list[int]:
        pass
