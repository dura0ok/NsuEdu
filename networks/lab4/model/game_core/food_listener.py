from abc import ABC, abstractmethod

from lib import GameStateCoord


class FoodListener(ABC):
    @abstractmethod
    def check_coordinate(self, x: int, y: int) -> str:
        pass

    @abstractmethod
    def get_field_string(self) -> str:
        pass

    @abstractmethod
    def set_food_point(self, coordinate: GameStateCoord) -> None:
        pass
