from lib import GameState, Direction, GameStateCoord, GameStateSnake, GameStateSnakeSnakeState


class Snake:
    def __init__(self, listener, width, height, player_id, coordinate_of_head=None, coordinate_of_tail=None,
                 snake=None):
        self.listener = listener
        self.player_id = player_id
        self.width = width
        self.height = height
        self.snake_coordinates = []

        if snake is None:
            head_direction = self.get_direction(coordinate_of_tail, coordinate_of_head)
            self.snake = GameStateSnake(
                player_id=player_id,
                state=GameStateSnakeSnakeState.ALIVE,
                head_direction=head_direction,
                points=[coordinate_of_head, coordinate_of_tail]
            )
            self.listener.set_snake_point(coordinate_of_head, player_id)
            self.listener.set_snake_point(coordinate_of_tail, player_id)
            self.snake_coordinates.extend([coordinate_of_head, coordinate_of_tail])
        else:
            self.snake = snake
            self.player_id = snake.player_id
            self.parse_snake_coordinates_from_proto()

    def parse_snake_coordinates_from_proto(self):
        self.snake_coordinates.clear()
        prev_coordinate = None
        for coordinate in self.snake.points:
            if not self.snake_coordinates:
                self.snake_coordinates.append(coordinate)
                prev_coordinate = coordinate
                self.listener.set_snake_point(prev_coordinate, self.player_id)
                continue

            offset_x, offset_y = coordinate.x, coordinate.y
            if offset_x != 0:
                if offset_x > 0:
                    for i in range(1, offset_x + 1):
                        prev_coordinate = GameStateCoord(self.listener.add_x(prev_coordinate.x, 1), prev_coordinate.y)
                        self.snake_coordinates.append(prev_coordinate)
                        self.listener.set_snake_point(prev_coordinate, self.player_id)
                else:
                    for i in range(-1, offset_x - 1, -1):
                        prev_coordinate = GameStateCoord(self.listener.subtract_x(prev_coordinate.x, 1),
                                                         prev_coordinate.y)
                        self.snake_coordinates.append(prev_coordinate)
                        self.listener.set_snake_point(prev_coordinate, self.player_id)
            else:
                if offset_y > 0:
                    for i in range(1, offset_y + 1):
                        prev_coordinate = GameStateCoord(prev_coordinate.x, self.listener.add_y(prev_coordinate.y, 1))
                        self.snake_coordinates.append(prev_coordinate)
                        self.listener.set_snake_point(prev_coordinate, self.player_id)
                else:
                    for i in range(-1, offset_y - 1, -1):
                        prev_coordinate = GameStateCoord(prev_coordinate.x,
                                                         self.listener.subtract_y(prev_coordinate.y, 1))
                        self.snake_coordinates.append(prev_coordinate)
                        self.listener.set_snake_point(prev_coordinate, self.player_id)

    def get_direction(self, from_, to):
        delta_x = to.x - from_.x
        if abs(delta_x) > 1:
            delta_x = 1 if delta_x < 0 else -1

        delta_y = to.y - from_.y
        if abs(delta_y) > 1:
            delta_y = 1 if delta_y < 0 else -1

        if delta_y == 1:
            return Direction.DOWN
        elif delta_y == -1:
            return Direction.UP
        elif delta_x == 1:
            return Direction.RIGHT
        elif delta_x == -1:
            return Direction.LEFT
        return None

    def get_delta_x(self, current_coordinate, previous_coordinate, direction):
        delta = current_coordinate - previous_coordinate
        if direction == Direction.LEFT:
            return delta if delta < 0 else delta - self.width
        elif direction == Direction.RIGHT:
            return delta if delta > 0 else delta + self.width
        return 0

    def get_delta_y(self, current_coordinate, previous_coordinate, direction):
        delta = current_coordinate - previous_coordinate
        if direction == Direction.UP:
            return delta if delta < 0 else delta - self.height
        elif direction == Direction.DOWN:
            return delta if delta > 0 else delta + self.height
        return 0

    def get_coordinates_for_protocol(self):
        coordinates_for_protocol = []
        prev_direction_to_tail = None

        head_direction = self.snake.head_direction
        if head_direction == Direction.UP:
            prev_direction_to_tail = Direction.DOWN
        elif head_direction == Direction.DOWN:
            prev_direction_to_tail = Direction.UP
        elif head_direction == Direction.LEFT:
            prev_direction_to_tail = Direction.RIGHT
        elif head_direction == Direction.RIGHT:
            prev_direction_to_tail = Direction.LEFT

        prev_point = self.snake_coordinates[0]
        coordination_num = len(self.snake_coordinates)
        iter_counter = 0
        last_added_coordinate = None

        for curr_point in self.snake_coordinates:
            iter_counter += 1

            if not coordinates_for_protocol:
                coordinates_for_protocol.append(curr_point)
                last_added_coordinate = curr_point
                prev_point = curr_point
                continue

            curr_direction_to_tail = self.get_direction(prev_point, curr_point)

            if prev_direction_to_tail != curr_direction_to_tail:
                assert curr_direction_to_tail is not None
                coordinates_for_protocol.append(GameStateCoord(
                    self.get_delta_x(prev_point.x, last_added_coordinate.x, prev_direction_to_tail),
                    self.get_delta_y(prev_point.y, last_added_coordinate.y, prev_direction_to_tail)
                ))
                last_added_coordinate = prev_point

            if coordination_num == iter_counter:
                coordinates_for_protocol.append(GameStateCoord(
                    self.get_delta_x(curr_point.x, last_added_coordinate.x, curr_direction_to_tail),
                    self.get_delta_y(curr_point.y, last_added_coordinate.y, curr_direction_to_tail)
                ))

            prev_direction_to_tail = curr_direction_to_tail
            prev_point = curr_point

        return coordinates_for_protocol

    def get_snake_proto(self):
        self.snake = GameStateSnake(
            player_id=self.snake.player_id,
            state=self.snake.state,
            head_direction=self.snake.head_direction,
            points=self.get_coordinates_for_protocol()
        )
        return self.snake

    def add_head(self, coordinate):
        self.snake_coordinates.insert(0, coordinate)
        self.listener.set_snake_point(coordinate, self.player_id)

    def delete_tail(self):
        tail = self.get_tail_coordinate()
        self.snake_coordinates.pop()
        self.listener.delete_snake_point(tail)

    def delete_snake(self):
        for c in self.snake_coordinates:
            self.listener.clear_snake_point(c)
        self.listener.snake_is_dead(self.snake.player_id, self.snake_coordinates)

    def get_head_coordinate(self):
        return self.snake_coordinates[0]

    def get_tail_coordinate(self):
        return self.snake_coordinates[-1]

    def contains_coordinate(self, point):
        return point in self.snake_coordinates

    def repeat_last_step(self):
        head_direction = self.snake.head_direction
        if head_direction == Direction.LEFT:
            self.make_left_move()
        elif head_direction == Direction.RIGHT:
            self.make_right_move()
        elif head_direction == Direction.DOWN:
            self.make_down_move()
        elif head_direction == Direction.UP:
            self.make_up_move()

    def make_right_move(self):
        if self.snake.head_direction != Direction.LEFT or len(self.snake_coordinates) == 1:
            head = self.get_head_coordinate()
            new_point = GameStateCoord(0, head.y) if head.x == self.width - 1 else GameStateCoord(head.x + 1, head.y)
            self.snake = GameStateSnake(
                player_id=self.snake.player_id,
                state=self.snake.state,
                head_direction=Direction.RIGHT,
                points=[new_point, self.get_tail_coordinate()]
            )
            self.listener.add_new_coordinate_on_step(self.player_id, new_point, self.get_tail_coordinate())
        else:
            self.repeat_last_step()

    def make_left_move(self):
        if self.snake.head_direction != Direction.RIGHT or len(self.snake_coordinates) == 1:
            head = self.get_head_coordinate()
            new_point = GameStateCoord(self.width - 1, head.y) if head.x == 0 else GameStateCoord(head.x - 1, head.y)
            self.snake = GameStateSnake(
                player_id=self.snake.player_id,
                state=self.snake.state,
                head_direction=Direction.LEFT,
                points=[new_point, self.get_tail_coordinate()]
            )
            self.listener.add_new_coordinate_on_step(self.player_id, new_point, self.get_tail_coordinate())
        else:
            self.repeat_last_step()

    def make_up_move(self):
        if self.snake.head_direction != Direction.DOWN or len(self.snake_coordinates) == 1:
            head = self.get_head_coordinate()
            new_point = GameStateCoord(head.x, self.height - 1) if head.y == 0 else GameStateCoord(head.x, head.y - 1)
            self.snake = GameStateSnake(
                player_id=self.snake.player_id,
                state=self.snake.state,
                head_direction=Direction.UP,
                points=[new_point, self.get_tail_coordinate()]
            )
            self.listener.add_new_coordinate_on_step(self.player_id, new_point, self.get_tail_coordinate())
        else:
            self.repeat_last_step()

    def make_down_move(self):
        if self.snake.head_direction != Direction.UP or len(self.snake_coordinates) == 1:
            head = self.get_head_coordinate()
            new_point = GameStateCoord(head.x, 0) if head.y == self.height - 1 else GameStateCoord(head.x, head.y + 1)
            self.snake = GameStateSnake(
                player_id=self.snake.player_id,
                state=self.snake.state,
                head_direction=Direction.DOWN,
                points=[new_point, self.get_tail_coordinate()]
            )
            self.listener.add_new_coordinate_on_step(self.player_id, new_point, self.get_tail_coordinate())
        else:
            self.repeat_last_step()

    def get_head_direction(self):
        return self.snake.head_direction
