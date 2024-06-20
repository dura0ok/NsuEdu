from lib import Direction, NodeRole, GameState
from model.action_updater.action_updater import ActionUpdater
from model.configuration import Configuration
from model.game_core.game_core import GameCore
from model.game_core.gamecore_listener import GameCoreListener
from model.inet.inet_controller import InetController
from model.inet.inet_controller_listener import InetControllerListener
from model.messages.announcement_message import make_key_for_announcement_msg
from model.players.players import Players
from model.players.players_listener import PlayersListener
from model.messages.announcement_message import AnnouncementMsg, make_key_for_announcement_msg


class Node(GameCoreListener, PlayersListener, InetControllerListener):
    def __init__(self, listener, name, port, player_type):
        self.listener = listener
        self.id = 0
        self.config = Configuration.default_config_builder()
        self.name = name
        self.port = port
        self.player_type = player_type
        self.node_role = NodeRole.VIEWER
        self.opportunity_to_join = True
        self.state_order = 0
        self.inet_controller = InetController(self, port, Configuration.ping_delay_ms, Configuration.node_timeout_ms)
        self.players = Players(self, self.inet_controller)
        self.inet_controller.attach_players(self.players)
        self.action_updater = None
        self.game_core = None

    def start_action_updater(self):
        self.action_updater = ActionUpdater(self.game_core, self.players, self.config.state_delay_ms)
        self.action_updater.start()

    def stop_action_updater(self):
        if self.action_updater is not None:
            self.action_updater.set_change_master_breakpoint()
        self.action_updater = None

    def get_game_state_data(self):
        return self.game_core.get_game_data().set_state_order(self.get_state_order()).set_config(self.config)

    def change_this_node_role(self, role, request_from_player):
        if self.node_role == role:
            return

        if role == NodeRole.MASTER:
            self.inet_controller.stop_multicast_receiver()
            self.inet_controller.start_multicast_publisher(self.id, self.config, self.players.get_players_list())
            self.start_action_updater()

            if self.node_role != NodeRole.MASTER:
                if self.players.get_number_of_players() > 0:
                    self.action_updater.add_snakes_to_action_updater()
                    self.players.change_player_role(self.id, NodeRole.MASTER, False)
        elif role in [NodeRole.NORMAL, NodeRole.DEPUTY]:
            self.stop_action_updater()
            self.inet_controller.stop_multicast_receiver()
            self.inet_controller.stop_multicast_publisher()
        elif role == NodeRole.VIEWER:
            self.stop_action_updater()
            self.inet_controller.stop_multicast_receiver()
            self.inet_controller.stop_multicast_publisher()

            if self.node_role == NodeRole.MASTER:
                self.players.change_player_role(self.id, NodeRole.VIEWER, False)

                if self.players.deputy is None:
                    self.end_the_game()
                else:
                    index = 0
                    for player in self.players.get_players_list():
                        if player.role == NodeRole.DEPUTY:
                            self.players.master = player.to_builder().set_role(NodeRole.MASTER).build()
                            self.players.change_player_in_list(index, self.players.master)
                            self.players.find_new_deputy()
                            break
                        index += 1
            elif role in [NodeRole.DEPUTY, NodeRole.NORMAL]:
                if request_from_player:
                    self.players.send_change_role_message(self.players.master, NodeRole.VIEWER, NodeRole.MASTER)

        print("Changed node role =", role)
        self.node_role = role
        self.players.set_node_role(self.node_role)

    def became_viewer(self):
        self.change_this_node_role(NodeRole.VIEWER, True)

    def node_snake_is_dead(self, player_id):
        print("Node snake is dead id =", player_id)
        self.action_updater.remove_player(player_id)

        if player_id == self.id:
            self.action_updater.set_change_master_breakpoint()
            self.change_this_node_role(NodeRole.VIEWER, False)
        else:
            self.players.change_player_role(player_id, NodeRole.VIEWER, True)

    def node_became_viewer(self, player_id):
        print("Node became a VIEWER: id =", player_id)
        self.players.change_player_role(player_id, NodeRole.VIEWER, False)

    def launch_game_core(self, node_id):
        self.id = node_id
        self.players.set_node_id(self.id)
        self.game_core = GameCore(self, self.config, self.id)

    def create_new_game(self, game_config):
        self.config = game_config
        self.config.state_delay_ms = int(self.listener.view.settings['state_delay_field'])
        self.launch_game_core(1)
        self.players.add_player(self.name, "127.0.0.43", self.port, self.node_role, self.player_type)
        self.change_this_node_role(NodeRole.MASTER, False)

    def join_the_game(self, key_game, new_node_role):
        self.inet_controller.stop_multicast_receiver()
        msg = self.inet_controller.get_announcement_msg(make_key_for_announcement_msg(key_game))
        if msg is not None:
            self.change_this_node_role(new_node_role, False)
            self.config = msg.game_message.games[0].config
            self.players.update_players_list(msg.game_message.games[0].players.players,
                                             msg.master.ip_address)
            self.listener.open_field_window(self.config.width, self.config.height)
            self.players.send_join_message(self.player_type, new_node_role == NodeRole.VIEWER, self.name, msg.master)

    def receive_game_state_msg(self, game_state: GameState, master_ip):
        if self.node_role != NodeRole.MASTER:
            self.players.update_players_list(game_state.players.players, master_ip)
            self.state_order = game_state.state_order
            self.game_core.update_game_state(game_state)
        else:
            print("I could not get state corrections!")

    def receive_role_change_msg(self, role_change_msg, sender_id):
        if role_change_msg.receiver_role != self.node_role:
            if self.node_role != NodeRole.MASTER:
                self.change_this_node_role(role_change_msg.receiver_role, False)
                if self.players.node.id != sender_id:
                    self.players.change_player_role(sender_id, role_change_msg.sender_role, False)
        else:
            if self.node_role == NodeRole.MASTER:
                self.node_became_viewer(sender_id)
            else:
                self.players.change_player_role(sender_id, role_change_msg.sender_role, False)

    def receive_steer_msg(self, direction, player_id):
        self.action_updater.add_action(player_id, direction)

    def receive_join_msg(self, name, ip, port, role, player_type):
        return self.players.add_player(name, ip, port, role, player_type)

    def add_player_in_game(self, new_player):
        if not self.game_core.add_new_player(new_player):
            return False
        self.action_updater.add_new_player(new_player)
        return True

    def add_one_point(self, node_player_id):
        self.players.add_one_point(node_player_id)

    @staticmethod
    def reverse_direction(direction):
        if direction == Direction.RIGHT:
            return Direction.LEFT
        elif direction == Direction.LEFT:
            return Direction.RIGHT
        elif direction == Direction.UP:
            return Direction.DOWN
        elif direction == Direction.DOWN:
            return Direction.UP
        return None

    def set_keyboard_action(self, direction):
        if self.node_role != NodeRole.VIEWER and self.reverse_direction(self.game_core.get_snake_direction(self.id)) != direction:
            if self.node_role == NodeRole.MASTER:
                self.action_updater.add_action(self.id, direction)
            elif self.node_role in [NodeRole.DEPUTY, NodeRole.NORMAL]:
                self.players.send_steer_message(direction)

    def get_state_order(self):
        self.state_order += 1
        return self.state_order

    def get_opportunity_to_join(self):
        self.opportunity_to_join = self.game_core.get_opportunity_to_join()
        return self.opportunity_to_join

    def update_field(self, field):
        self.listener.update_field(field, self.players.get_scores(), str(self.node_role))

    def update_find_game_list(self, games):
        self.listener.update_find_game_list(games)

    def end_the_game(self):
        self.stop_action_updater()
        self.inet_controller.socket.close()
        self.inet_controller.stop_multicast_publisher()
        self.inet_controller.stop_multicast_receiver()
        self.inet_controller.interrupt_unicast()
