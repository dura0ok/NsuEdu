from abc import abstractmethod, ABC

from lib import GameStateCoord


class SnakeListener(ABC):
    @abstractmethod
    def check_coordinate(self, x, y):
        pass

    @abstractmethod
    def add_new_coordinate_on_step(
            self, node_player_id: int, new_coordinate: GameStateCoord, del_coordinate: GameStateCoord
    ):
        pass

    @abstractmethod
    def set_snake_point(self, coordinate: GameStateCoord, player_id: int):
        pass

    @abstractmethod
    def clear_snake_point(self, coordinate: GameStateCoord):
        pass

    @abstractmethod
    def delete_snake_point(self, coordinate: GameStateCoord):
        pass

    @abstractmethod
    def snake_is_dead(self, snake_player_id, snake_coordinates: list[GameStateCoord]):
        pass

    @abstractmethod
    def add_x(self, x: int, delta: int):
        pass

    @abstractmethod
    def subtract_x(self, x: int, delta: int):
        pass

    @abstractmethod
    def add_y(self, y: int, delta: int):
        pass

    @abstractmethod
    def subtract_y(self, y: int, delta: int):
        pass
