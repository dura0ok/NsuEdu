from random import randint
from threading import Lock
from typing import Union, List, Optional

from lib import GameConfig, GameStateCoord, Direction
from model.action_updater.game_core_for_action_updater import GameCoreForActionUpdater
from model.game_core.food import Food
from model.game_core.food_listener import FoodListener
from model.game_core.gamecore_listener import GameCoreListener
from model.game_core.point_type import PointType, build_coordinate
from model.game_core.snake import Snake
from model.game_core.snake_listener import SnakeListener
from model.players.field_point import FieldPoint


class GameCore(SnakeListener, FoodListener, GameCoreForActionUpdater):
    def __init__(self, listener: GameCoreListener, config: GameConfig, node_player_id: int):
        self.listener = listener
        self.width = config.width
        self.height = config.height
        self.field = [['-' for _ in range(self.width)] for _ in range(self.height)]
        self.clear_field()
        self.snake_map: dict[int, Snake] = {}
        self.node_player_id = node_player_id
        self.food = Food(self, self.width, self.height, config.food_static, config.state_delay_ms)
        self.id_of_snake_for_delete_coords: dict[int, GameStateCoord] = {}
        self.id_list_of_new_coords: dict[GameStateCoord, list[int]] = {}
        self.prev_field = "-" * (self.height * self.width)
        self.lock = Lock()

    def get_opportunity_to_join(self) -> bool:
        return self.get_field_string().find('-') != -1

    def get_snake_direction(self, player_id: int) -> Direction:
        return self.snake_map[player_id].get_snake_proto().head_direction

    def add_new_player(self, player_id: int) -> bool:
        if not self.create_new_snake(player_id):
            return False
        self.food.update_number_of_food(len(self.snake_map))
        self.food.add_all_not_added_food()
        return True

    def create_new_snake(self, player_id: int) -> bool:
        index_of_empty = self.get_field_string().find('-')
        if index_of_empty == -1:
            return False
        x_head = index_of_empty % self.width
        y_head = index_of_empty // self.height
        head = GameStateCoord(x_head, y_head)
        tail = self.find_tail_coordination(x_head, y_head)
        iteration = 0
        while not tail:
            if iteration == 20:
                return False
            rand_x = randint(0, self.width - 1)
            rand_y = randint(0, self.height - 1)
            with self.lock:
                if self.field[rand_y][rand_x] == '-':
                    rand_head = GameStateCoord(rand_x, rand_y)
                    rand_tail = self.find_tail_coordination(rand_x, rand_y)
                    if rand_tail:
                        head = rand_head
                        tail = rand_tail
                        break
            iteration += 1
        if not tail:
            return False
        new_snake = Snake(self, self.width, self.height, player_id, head, tail)
        self.snake_map[player_id] = new_snake
        return True

    def update_game_state(self, game_state) -> None:
        self.clear_field()
        snakes_list = game_state.snakes
        self.snake_map.clear()
        for snake in snakes_list:
            self.snake_map[snake.player_id] = Snake(self, self.width, self.height, snake.player_id, snake=snake)
        self.food.update_food(game_state.foods)
        self.listener.update_field(self.get_field_points())

    def get_list_with_snakes_proto(self):
        snakes_proto = []
        snakes_objects = list(self.snake_map.values())
        for snake in snakes_objects:
            snakes_proto.append(snake.get_snake_proto())
        return snakes_proto

    def get_game_data(self):
        return {
            'snakes': self.get_list_with_snakes_proto(),
            'foods': self.food.get_list_of_food_coordinates()
        }

    def make_action(self, player_id: int, direction: Direction) -> None:
        if direction == Direction.LEFT:
            self.snake_map[player_id].make_left_move()
        elif direction == Direction.RIGHT:
            self.snake_map[player_id].make_right_move()
        elif direction == Direction.DOWN:
            self.snake_map[player_id].make_down_move()
        elif direction == Direction.UP:
            self.snake_map[player_id].make_up_move()

    def clear_field(self) -> None:
        self.field = [['-' for _ in range(self.width)] for _ in range(self.height)]

    def add_x(self, x: int, delta: int) -> int:
        new_x = x + delta
        return new_x if new_x < self.width else new_x - self.width

    def subtract_x(self, x: int, delta: int) -> int:
        new_x = x - delta
        return new_x if new_x >= 0 else new_x + self.width

    def add_y(self, y: int, delta: int) -> int:
        new_y = y + delta
        return new_y if new_y < self.height else new_y - self.height

    def subtract_y(self, y: int, delta: int) -> int:
        new_y = y - delta
        return new_y if new_y >= 0 else new_y + self.height

    def check_coordinate(self, x: int, y: int) -> Union[PointType, None]:
        sym = self.field[y][x]
        if sym == '-' or sym == '.':
            return PointType.EMPTY
        elif sym == '#' or sym == '&':
            return PointType.SNAKE
        elif sym == '*':
            return PointType.FOOD
        return None

    def set_food_point(self, coordinate: GameStateCoord) -> None:
        x, y = coordinate.x, coordinate.y
        self.field[y][x] = '*'

    def add_one_point(self, node_player_id: int) -> None:
        self.listener.add_one_point(node_player_id)

    def add_new_coordinate_on_step(
            self, node_player_id: int, new_coordinate: GameStateCoord, del_coordinate: GameStateCoord
    ) -> None:
        point_type = self.check_coordinate(new_coordinate.x, new_coordinate.y)
        if point_type != PointType.FOOD:
            self.id_of_snake_for_delete_coords[node_player_id] = del_coordinate
        else:
            self.add_one_point(node_player_id)
        if new_coordinate in self.id_list_of_new_coords:
            self.id_list_of_new_coords[new_coordinate].append(node_player_id)
        else:
            self.id_list_of_new_coords[new_coordinate] = [node_player_id]

    def set_snake_point(self, coordinate: GameStateCoord, player_id: int) -> None:
        x, y = coordinate.x, coordinate.y
        if self.field[y][x] == '*':
            self.food.food_was_eaten(coordinate)
        if self.node_player_id == player_id:
            self.field[y][x] = '&'
        else:
            self.field[y][x] = '#'
        self.set_sym_in_square('.', x, y)

    def clear_snake_point(self, coordinate: GameStateCoord) -> None:
        x, y = coordinate.x, coordinate.y
        self.field[y][x] = '-'
        self.set_sym_in_square('-', x, y)

    def delete_snake_point(self, coordinate: GameStateCoord) -> None:
        x, y = coordinate.x, coordinate.y
        prev_x, prev_y = 0, 0
        sym = self.field[self.subtract_y(y, 1)][x]
        if sym == '#' or sym == '&':
            prev_x, prev_y = x, self.subtract_y(y, 1)
        else:
            sym = self.field[self.add_y(y, 1)][x]
            if sym == '#' or sym == '&':
                prev_x, prev_y = x, self.add_y(y, 1)
            else:
                sym = self.field[y][self.subtract_x(x, 1)]
                if sym == '#' or sym == '&':
                    prev_x, prev_y = self.subtract_x(x, 1), y
                else:
                    sym = self.field[y][self.add_x(x, 1)]
                    if sym == '#' or sym == '&':
                        prev_x, prev_y = self.add_x(x, 1), y
        self.set_sym_in_square('-', x, y)
        self.set_sym_in_square('.', prev_x, prev_y)
        self.field[y][x] = '.'

    def snake_is_dead(self, snake_player_id: int, snake_coordinates: list[GameStateCoord]) -> None:
        self.food.turn_snake_into_food(snake_coordinates)
        self.snake_map.pop(snake_player_id, None)
        self.food.update_number_of_food(len(self.snake_map))
        self.listener.node_snake_is_dead(snake_player_id)
        print(f"Snake {snake_player_id} is dead!")

    def get_field_string(self) -> str:
        return ''.join([''.join(row) for row in self.field])

    def get_list_of_snakes_id(self) -> list[int]:
        return list(self.snake_map.keys())

    def update_field(self) -> None:
        for player_id, del_coordinate in self.id_of_snake_for_delete_coords.items():
            self.snake_map[player_id].delete_tail()
        dead_snakes = []
        for coordinate, id_list in self.id_list_of_new_coords.items():
            if len(id_list) == 1:
                player_id = id_list[0]
                point_type = self.check_coordinate(coordinate.x, coordinate.y)
                if point_type != PointType.SNAKE:
                    curr_snake = self.snake_map[player_id]
                    curr_snake.add_head(coordinate)
                else:
                    for some_id in self.snake_map:
                        if self.snake_map[some_id].contains_coordinate(coordinate):
                            self.add_one_point(some_id)
                            break
                    dead_snakes.append(player_id)
            else:
                dead_snakes.extend(id_list)
        is_master_snake_dead = self.node_player_id in dead_snakes
        for player_id in dead_snakes:
            if is_master_snake_dead:
                self.snake_map[player_id].delete_snake()
        self.id_of_snake_for_delete_coords.clear()
        self.id_list_of_new_coords.clear()
        self.listener.update_field(self.get_field_points())

    def set_sym_in_square(self, sym: str, x_center: int, y_center: int) -> None:
        start_x = self.subtract_x(x_center, (5 - 1) // 2)
        start_y = self.subtract_y(y_center, (5 - 1) // 2)
        for i in range(5):
            new_x = self.add_x(start_x, i)
            for j in range(5):
                new_y = self.add_y(start_y, j)
                if self.field[new_y][new_x] not in ('#', '&', '*'):
                    self.field[new_y][new_x] = sym

    def get_field_points(self) -> List[FieldPoint]:
        field_points = []
        index = -1
        self.prev_field = self.get_field_string()
        for i in range(self.height):
            for j in range(self.width):
                sym = self.field[i][j]
                index += 1
                # if sym == self.prev_field[index]:
                #     continue
                coordinate = build_coordinate(j, i)
                field_points.append(FieldPoint(sym, coordinate))

        return field_points

    @staticmethod
    def get_random_direction() -> Direction:
        dir_value = randint(1, 4)
        return Direction(dir_value)

    def find_tail_coordination(self, x_head: int, y_head: int) -> Optional[GameStateCoord]:
        success = False
        direction_set = set()
        x_tail, y_tail = 0, 0

        while not success:
            if len(direction_set) == 4:
                return None

            direction = self.get_random_direction()
            if direction == Direction.LEFT:
                direction_set.add(Direction.LEFT)
                x_tail = self.add_x(x_head, 1)
                y_tail = y_head
                if self.field[y_tail][x_tail] == '-':
                    success = True
            elif direction == Direction.RIGHT:
                direction_set.add(Direction.RIGHT)
                x_tail = self.subtract_x(x_head, 1)
                y_tail = y_head
                if self.field[y_tail][x_tail] == '-':
                    success = True
            elif direction == Direction.UP:
                direction_set.add(Direction.UP)
                x_tail = x_head
                y_tail = self.add_y(y_head, 1)
                if self.field[y_tail][x_tail] == '-':
                    success = True
            elif direction == Direction.DOWN:
                direction_set.add(Direction.DOWN)
                x_tail = x_head
                y_tail = self.subtract_y(y_head, 1)
                if self.field[y_tail][x_tail] == '-':
                    success = True

        return build_coordinate(x_tail, y_tail)
