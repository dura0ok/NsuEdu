from abc import ABC, abstractmethod

from model.players.field_point import FieldPoint


class NodeListener(ABC):
    @abstractmethod
    def update_field(self, field: list[FieldPoint], scores_table: list[str], node_role: str) -> None:
        pass

    @abstractmethod
    def update_find_game_list(self, games: list[str]) -> None:
        pass

    @abstractmethod
    def open_field_window(self, width_field: int, height_field: int) -> None:
        pass
