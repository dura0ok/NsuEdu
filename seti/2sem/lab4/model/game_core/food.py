import random
from random import randint
from typing import Any

from lib import GameStateCoord
from model.game_core.food_listener import FoodListener
from model.game_core.point_type import PointType, build_coordinate


class Food:
    def __init__(self, listener, width: int, height: int, food_static: int, state_delay_ms: int):
        self.listener: FoodListener = listener
        self.width: int = width
        self.height: int = height
        self.food_static: int = food_static
        self.state_delay_ms: int = state_delay_ms
        self.number_of_alive_snakes: int = 0
        self.number_of_food: int = 0
        self.food_coordinates: set[GameStateCoord] = set()
        self.not_added_food: int = 0

    def add_new_food(self, new_food: GameStateCoord) -> None:
        self.food_coordinates.add(new_food)
        self.listener.set_food_point(new_food)

    def randomize_food_position(self) -> Any | None:
        success: bool = False
        field: str = self.listener.get_field_string()
        free_place: int = field.find('-')
        if free_place == -1:
            return None

        x: int = field.find('-') % self.width
        y: int = field.find('-') // self.height
        iter_count: int = 0

        while not success:
            rand_x: int = randint(0, self.width - 1)
            rand_y: int = randint(0, self.height - 1)

            if self.listener.check_coordinate(rand_x, rand_y) == PointType.EMPTY:
                x = rand_x
                y = rand_y
                success = True

            iter_count += 1
            if iter_count == 20:
                break

        return build_coordinate(x, y)

    def update_number_of_food(self, number_of_alive_snakes: int) -> None:
        self.number_of_alive_snakes = number_of_alive_snakes
        new_number_of_food: int = self.food_static + self.number_of_alive_snakes
        if new_number_of_food > self.number_of_food:
            self.not_added_food += new_number_of_food - self.number_of_food

        self.number_of_food = new_number_of_food

    def update_food(self, food: list[GameStateCoord]) -> None:
        self.food_coordinates = set(food)
        for coordinate in self.food_coordinates:
            self.listener.set_food_point(coordinate)

    def add_all_not_added_food(self) -> None:
        local_not_added_food: int = self.not_added_food
        for _ in range(local_not_added_food):
            new_food: GameStateCoord = self.randomize_food_position()
            if new_food is not None:
                self.not_added_food -= 1
                self.add_new_food(new_food)

    def food_was_eaten(self, ate_food: GameStateCoord) -> None:
        self.food_coordinates.remove(ate_food)
        self.not_added_food += 1
        self.add_all_not_added_food()

    def turn_snake_into_food(self, snake_coordinates: list[GameStateCoord]) -> None:
        for coordinate in snake_coordinates:
            if random.random():
                self.add_new_food(coordinate)

    def get_list_of_food_coordinates(self) -> list[GameStateCoord]:
        return list(self.food_coordinates)
