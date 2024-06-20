from abc import abstractmethod, ABC

from model.players.field_point import FieldPoint


class GameCoreListener(ABC):
    @abstractmethod
    def update_field(self, field: list[FieldPoint]) -> None:
        pass

    @abstractmethod
    def node_snake_is_dead(self, player_id: int) -> None:
        pass

    @abstractmethod
    def add_one_point(self, node_player_id: int) -> None:
        pass
