from abc import abstractmethod, ABC

from lib import GameState, GameMessageRoleChangeMsg, Direction, NodeRole, PlayerType


class UnicastReceiverListener(ABC):
    @abstractmethod
    def receive_game_state_msg(self, game_state: GameState, master_ip: str) -> None:
        pass

    @abstractmethod
    def receive_role_change_msg(self, role_change_msg: GameMessageRoleChangeMsg, sender_id: int) -> None:
        pass

    @abstractmethod
    def receive_steer_msg(self, direction: Direction, player_id: int) -> None:
        pass

    @abstractmethod
    def receive_join_msg(self, name: str, ip: str, port: int, role: NodeRole, player_type: PlayerType) -> int:
        pass
