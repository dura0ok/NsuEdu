from dataclasses import dataclass

from lib import GameConfig


@dataclass
class Configuration:
    width: int = 40
    height: int = 30
    food_per_player: float = 1
    ping_delay_ms: int = 100
    node_timeout_ms: int = 800

    @staticmethod
    def default_config_builder():
        return Configuration.config_builder(Configuration.width, Configuration.height, 2, 1000)

    @staticmethod
    def config_builder(width, height, food_static, state_delay):
        return GameConfig(
            width=width,
            height=height,
            food_static=food_static,
            state_delay_ms=state_delay,
        )
