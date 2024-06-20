from enum import Enum

from lib import GameStateCoord


class PointType(Enum):
    EMPTY = 0
    SNAKE = 1
    FOOD = 2


def build_coordinate(x: int, y: int) -> GameStateCoord:
    return GameStateCoord(x, y)
