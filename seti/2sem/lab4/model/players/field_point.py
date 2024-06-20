from dataclasses import dataclass

from lib import GameStateCoord


@dataclass
class FieldPoint:
    sym: str
    coordinate: GameStateCoord
