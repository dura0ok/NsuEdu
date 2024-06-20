from abc import ABC, abstractmethod
from typing import List, TYPE_CHECKING

from model.players.field_point import FieldPoint

if TYPE_CHECKING:
    from model.presenter import Presenter


class View(ABC):
    @abstractmethod
    def attach_presenter(self, presenter: "Presenter") -> None:
        pass

    @abstractmethod
    def change_visible(self, var: bool) -> None:
        pass

    @abstractmethod
    def update_game_view(self, field: list[FieldPoint], scores_table: List[str], node_role: str) -> None:
        pass

    @abstractmethod
    def update_find_game_list(self, games: List[str]) -> None:
        pass

    @abstractmethod
    def open_field(self, width_field: int, height_field: int) -> None:
        pass
