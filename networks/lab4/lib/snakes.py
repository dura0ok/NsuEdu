# Generated by the protocol buffer compiler.  DO NOT EDIT!
# sources: snakes.proto
# plugin: python-betterproto
from dataclasses import dataclass
from typing import List

import betterproto


class NodeRole(betterproto.Enum):
    """Роль узла в топологии связей узлов в пределах игры"""

    NORMAL = 0
    MASTER = 1
    DEPUTY = 2
    VIEWER = 3


class PlayerType(betterproto.Enum):
    """Тип игрока"""

    HUMAN = 0
    ROBOT = 1


class Direction(betterproto.Enum):
    UP = 1
    DOWN = 2
    LEFT = 3
    RIGHT = 4


class GameStateSnakeSnakeState(betterproto.Enum):
    ALIVE = 0
    ZOMBIE = 1


@dataclass
class GamePlayer(betterproto.Message):
    """Игрок"""

    name: str = betterproto.string_field(1)
    id: int = betterproto.int32_field(2)
    ip_address: str = betterproto.string_field(3)
    port: int = betterproto.int32_field(4)
    role: "NodeRole" = betterproto.enum_field(5)
    type: "PlayerType" = betterproto.enum_field(6)
    score: int = betterproto.int32_field(7)


@dataclass
class GameConfig(betterproto.Message):
    """Параметры идущей игры (не должны меняться в процессе игры)"""

    width: int = betterproto.int32_field(1)
    height: int = betterproto.int32_field(2)
    food_static: int = betterproto.int32_field(3)
    state_delay_ms: int = betterproto.int32_field(5)


@dataclass
class GamePlayers(betterproto.Message):
    """Игроки конкретной игры"""

    players: List["GamePlayer"] = betterproto.message_field(1)


@dataclass
class GameState(betterproto.Message):
    """Текущее состояние игрового поля"""

    state_order: int = betterproto.int32_field(1)
    snakes: List["GameStateSnake"] = betterproto.message_field(2)
    foods: List["GameStateCoord"] = betterproto.message_field(3)
    players: "GamePlayers" = betterproto.message_field(4)


@dataclass(unsafe_hash=True)
class GameStateCoord(betterproto.Message):
    """
    Координаты в пределах игрового поля, либо относительное смещение координат.
    Левая верхняя клетка поля имеет координаты (x=0, y=0). Направление смещения
    задаётся знаком чисел.
    """

    x: int = betterproto.sint32_field(1)
    y: int = betterproto.sint32_field(2)


@dataclass
class GameStateSnake(betterproto.Message):
    """Змея"""

    player_id: int = betterproto.int32_field(1)
    # Список "ключевых" точек змеи. Первая точка хранит координаты головы змеи.
    # Каждая следующая - смещение следующей "ключевой" точки относительно
    # предыдущей, в частности последняя точка хранит смещение хвоста змеи
    # относительно предыдущей "ключевой" точки.
    points: List["GameStateCoord"] = betterproto.message_field(2)
    state: "GameStateSnakeSnakeState" = betterproto.enum_field(3)
    head_direction: "Direction" = betterproto.enum_field(4)


@dataclass
class GameAnnouncement(betterproto.Message):
    players: "GamePlayers" = betterproto.message_field(1)
    config: "GameConfig" = betterproto.message_field(2)
    can_join: bool = betterproto.bool_field(3)
    game_name: str = betterproto.string_field(4)


@dataclass
class GameMessage(betterproto.Message):
    """Общий формат любого UDP-сообщения"""

    msg_seq: int = betterproto.int64_field(1)
    sender_id: int = betterproto.int32_field(10)
    receiver_id: int = betterproto.int32_field(11)
    ping: "GameMessagePingMsg" = betterproto.message_field(2, group="Type")
    steer: "GameMessageSteerMsg" = betterproto.message_field(3, group="Type")
    ack: "GameMessageAckMsg" = betterproto.message_field(4, group="Type")
    state: "GameMessageStateMsg" = betterproto.message_field(5, group="Type")
    announcement: "GameMessageAnnouncementMsg" = betterproto.message_field(6, group="Type")
    join: "GameMessageJoinMsg" = betterproto.message_field(7, group="Type")
    error: "GameMessageErrorMsg" = betterproto.message_field(8, group="Type")
    role_change: "GameMessageRoleChangeMsg" = betterproto.message_field(9, group="Type")
    discover: "GameMessageDiscoverMsg" = betterproto.message_field(12, group="Type")


@dataclass
class GameMessagePingMsg(betterproto.Message):
    """Ничего не меняем, просто говорим, что мы живы"""

    pass


@dataclass
class GameMessageSteerMsg(betterproto.Message):
    """Не-центральный игрок просит повернуть голову змеи"""

    direction: "Direction" = betterproto.enum_field(1)


@dataclass
class GameMessageAckMsg(betterproto.Message):
    """Подтверждение сообщения с таким же seq"""

    seq: int = betterproto.int64_field(1)
    sender_id: int = betterproto.int64_field(2)
    player_id: int = betterproto.int64_field(3)


@dataclass
class GameMessageStateMsg(betterproto.Message):
    """Центральный узел сообщает остальным игрокам состояние игры"""

    state: "GameState" = betterproto.message_field(1)


@dataclass
class GameMessageAnnouncementMsg(betterproto.Message):
    """
    Уведомление об идущих играх, регулярно отправляется multicast-ом или в
    ответ на DiscoverMsg
    """

    games: List["GameAnnouncement"] = betterproto.message_field(1)


@dataclass
class GameMessageDiscoverMsg(betterproto.Message):
    """Запрос информации об идущих играх"""

    pass


@dataclass
class GameMessageJoinMsg(betterproto.Message):
    """Новый игрок хочет присоединиться к идущей игре"""

    player_type: "PlayerType" = betterproto.enum_field(1)
    player_name: str = betterproto.string_field(3)
    game_name: str = betterproto.string_field(4)
    requested_role: "NodeRole" = betterproto.enum_field(5)


@dataclass
class GameMessageErrorMsg(betterproto.Message):
    """
    Ошибка операции (например отказ в присоединении к игре, т.к. нет места на
    поле)
    """

    error_message: str = betterproto.string_field(1)


@dataclass
class GameMessageRoleChangeMsg(betterproto.Message):
    """
    Сообщение о смене роли: 1. от заместителя другим игрокам о том, что пора
    начинать считать его главным (sender_role = MASTER) 2. от осознанно
    выходящего игрока (sender_role = VIEWER) 3. от главного к умершему игроку
    (receiver_role = VIEWER) 4. в комбинации с 1,2 или отдельно от них:
    назначение кого-то заместителем (receiver_role = DEPUTY) 5. в комбинации с
    2 от главного узла заместителю о том, что он становится главным
    (receiver_role = MASTER)
    """

    sender_role: "NodeRole" = betterproto.enum_field(1)
    receiver_role: "NodeRole" = betterproto.enum_field(2)
