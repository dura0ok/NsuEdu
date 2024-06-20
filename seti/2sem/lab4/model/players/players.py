from lib import NodeRole, GamePlayer
from lib.snakes import *
from model.action_updater.players_for_action_updater import PlayersForActionUpdater
from model.inet.players_for_inet import PlayersForInet
from model.messages.message_builder import MessageBuilder
import time


class Players(PlayersForActionUpdater, PlayersForInet):
    def __init__(self, listener, inet_controller):
        self.listener = listener
        self.inet_controller = inet_controller
        self.player_list = []
        self.master = None
        self.deputy = None
        self.new_player_id_counter = 1
        self.node_id = 0
        self.node_role = NodeRole.VIEWER

    def set_node_id(self, node_id):
        self.node_id = node_id
        print('!!!!!!!!!!', node_id)

    def set_node_role(self, node_role):
        self.node_role = node_role


    def add_player(self, name, ip, port, role, player_type):
        new_player_id = self.get_player_id_by_ip_and_port(ip, port)
        if new_player_id == 0:
            new_player_id = self.new_player_id_counter
            self.new_player_id_counter += 1
            player_builder = {
                "name": name,
                "id": new_player_id,
                "ip_address": ip,
                "port": port,
                "type": player_type,
                "score": 0
            }
            new_player = GamePlayer(**player_builder)
            if role == NodeRole.MASTER:
                self.master = new_player
            self.player_list.append(new_player)
            if role != NodeRole.VIEWER:
                if not self.listener.add_player_in_game(new_player_id):
                    self.send_error_message(new_player_id)
                    self.disconnect_player(new_player_id)
                    print('error')
                    return -1
        print('player id:', new_player_id)
        return new_player_id

    def add_one_point(self, node_player_id):
        for player in self.player_list:
            if player.id == node_player_id:
                player.score += 1
                break

    def change_player_in_list(self, index, new_player):
        self.player_list[index] = new_player

    def update_players_list(self, players_list, ip):
        index = 0
        new_master = self.master
        self.deputy = None
        for player in self.player_list:
            if player.role == int(NodeRole.MASTER):
                if player.id != self.node_id:
                    player.ip_address = ip
                else:
                    new_master = player
            elif player.role == int(NodeRole.DEPUTY):
                self.deputy = player
            elif player.role == int(NodeRole.VIEWER):
                if self.master is not None and player.id == self.master.id:
                    player.ip_address = ip
            print(player.id != self.node_id)
            index += 1
        self.master = new_master

    def delete_player(self, player_id):
        index = 0
        for player in self.player_list:
            if player.id == player_id:
                break
            index += 1
        del self.player_list[index]
        print("Players: delete dead node =", player_id)
        if self.node_role != NodeRole.MASTER:
            self.inet_controller.remove_player_from_ping(player_id)

    def disconnect_player(self, player_id):
        player = self.get_game_player_by_id(player_id)
        if player is not None:
            if self.node_role == NodeRole.MASTER:
                print(f"Node {player.id} {player.ip_address}:{player.port} was disconnected")
                self.delete_player(player_id)
                if player.role == NodeRole.DEPUTY:
                    self.find_new_deputy()
            elif self.node_role == NodeRole.DEPUTY:
                if player.role == NodeRole.MASTER:
                    self.listener.change_this_node_role(NodeRole.MASTER, False)
                    max_id = max(player.id for player in self.player_list)
                    self.new_player_id_counter = max_id + 1
                    self.delete_player(player_id)
                    self.send_change_role_to_all_players(NodeRole.MASTER)
                    self.find_new_deputy()
            elif self.node_role == NodeRole.NORMAL:
                self.delete_player(self.master.id)
                self.change_player_role(self.deputy.id, NodeRole.MASTER, False)

    def find_new_deputy(self):
        index = 0
        for player in self.player_list:
            if player.role == NodeRole.NORMAL:
                self.deputy = player
                self.deputy.role=NodeRole.DEPUTY
                print("Deputy was changed: deputyId=", self.deputy.id)
                self.player_list[index] = self.deputy
                self.send_change_role_message(self.deputy, NodeRole.MASTER, NodeRole.DEPUTY)
                return
            index += 1
        self.deputy = None

    def initiate_deputy_player(self):
        if self.deputy is None:
            self.find_new_deputy()

    def transfer_role_of_the_master(self):
        if self.master.id != self.node_id:
            self.send_change_role_message(self.master, NodeRole.VIEWER, NodeRole.MASTER)
            print("Change role message was sent to nodeId=", self.master.id)

    def change_player_role(self, player_id, role, to_notify):
        index = 0
        for player in self.player_list:
            if player.id == player_id:
                prev_role = player.role
                if prev_role == role:
                    return
                if player.role == 0:
                    if not self.listener.add_player_in_game(player_id):
                        self.send_error_message(player_id)
                        self.disconnect_player(player_id)
                        return
                player.role=role
                if role == NodeRole.MASTER:
                    self.master = self.player_list[index]
                    print("Players: changed master =", player_id)
                if prev_role == NodeRole.DEPUTY and self.master.id == self.node_id:
                    self.find_new_deputy()
                if to_notify and player.id != self.node_id:
                    self.send_change_role_message(player, NodeRole.MASTER, role)
                break
            index += 1

    def send_change_role_to_all_players(self, changed_role):
        for player in self.player_list:
            if player.id_adres != self.node_id:
                self.send_change_role_message(player, changed_role, player.role)

    def send_game_state_to_all_players(self):
        game_players = GamePlayers(players=self.player_list)
        food = self.listener.game_core.food.get_list_of_food_coordinates()
        snakes = self.listener.game_core.get_list_with_snakes_proto()
        game_state = GameState(players=game_players, foods=food, snakes=snakes)
        for player in self.player_list:
            if player.id != self.node_id:
                print('sended to', player.id)
                self.send_game_state_message(player, game_state)

    def send_error_message(self, player_id):
        player = self.get_game_player_by_id(player_id)
        if player is not None:
            self.inet_controller.send_message(
                player, MessageBuilder.error_msg_builder(
                    "There is not enough space on the playing field for you.\nTry again later!\nHave a nice try!",
                    self.node_id, player_id))

    def send_ping(self, player_id):
        player = self.get_game_player_by_id(player_id)
        if player is not None:
            self.inet_controller.send_message(player, MessageBuilder.ping_msg_builder(self.node_id, player_id))

    def send_join_message(self, player_type, only_view, name, master):
        self.master = master
        self.inet_controller.send_message(master,
                                              MessageBuilder.join_msg_builder(player_type, name,
                                                                              'self.node_id', self.node_role, 'master.id'))

    def send_steer_message(self, direction):
        if self.master is not None:
            self.inet_controller.send_message(self.master,
                                              MessageBuilder.steer_msg_builder(direction, self.node_id, self.master.id))

    def send_game_state_message(self, receiver, game_state):
        self.inet_controller.send_message(receiver,
                                          MessageBuilder.state_msg_builder(game_state, self.node_id, receiver.id))

    def send_change_role_message(self, receiver, sender_role, receiver_role):
        self.inet_controller.send_message(receiver, MessageBuilder.role_changing_msg_builder(sender_role, receiver_role,
                                                                                             self.node_id, receiver.id))

    def get_number_of_players(self):
        return len(self.player_list)

    def get_players_list(self):
        return self.player_list

    def get_game_player_by_id(self, player_id):
        for player in self.player_list:
            if player.id == player_id:
                return player
        return None

    def get_scores(self):
        scores_table = []
        for player in self.player_list:
            scores_table.append(f"{player.name}:{player.score}")
        return scores_table

    def get_player_id_by_ip_and_port(self, ip, port):
        for player in self.player_list:
            if player.ip_address == ip and player.port == port:
                return player.id
        return 0
