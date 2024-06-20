from lib import PlayerType, NodeRole, GameConfig, Direction
from model.configuration import Configuration
from model.node import Node
from model.players.field_point import FieldPoint


class Presenter:
    def __init__(self, view):
        self.view = view
        self.node: Node | None = None
        self.view.attach_presenter(self)

    def launch_ui(self) -> None:
        self.view.change_visible(True)

    def find_the_game(self, name: str, port: int, player_type: PlayerType) -> None:
        self.node = Node(self, name, port, player_type)

    def join_the_game(self, node_role: NodeRole, game_key: str) -> None:
        self.node.join_the_game(game_key, node_role)

    def create_new_game(self, config: GameConfig, name: str, port: int, player_type: PlayerType) -> None:
        self.node = Node(self, name, port, player_type)
        self.node.create_new_game(config)

    def start_the_game(self, name: str, port: int, player_type: PlayerType) -> None:
        config: GameConfig = Configuration.default_config_builder()
        self.create_new_game(config, name, port, player_type)

    def start_the_game_with_parameters(self, name: str, port: int, player_type: PlayerType, width: int, height: int,
                                       food_static: int, state_delay: int) -> None:
        config: GameConfig = Configuration.config_builder(width, height, food_static, state_delay)
        self.create_new_game(config, name, port, player_type)

    def change_role_on_viewer(self) -> None:
        self.node.became_viewer()

    def make_right_move(self) -> None:
        self.node.set_keyboard_action(Direction.RIGHT)

    def make_left_move(self) -> None:
        self.node.set_keyboard_action(Direction.LEFT)

    def make_up_move(self) -> None:
        self.node.set_keyboard_action(Direction.UP)

    def make_down_move(self) -> None:
        self.node.set_keyboard_action(Direction.DOWN)

    def end_the_session(self) -> None:
        # TODO: make node closing
        pass

    def open_field_window(self, width_field: int, height_field: int) -> None:
        self.view.open_field(width_field, height_field)

    def update_field(self, field: list[FieldPoint], scores_table: list[str], node_role: str) -> None:
        self.view.update_game_view(field, scores_table, node_role)

    def update_find_game_list(self, games: list[str]) -> None:
        self.view.update_find_game_list(games)

    def leave_the_game(self) -> None:
        self.node.end_the_game()
        print('leaved')
        self.view.back_to_start_menu()
