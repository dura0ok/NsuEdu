import socket, time
import traceback
from collections import OrderedDict
from threading import Lock  # Import the Lock class for synchronization

from model.inet.multicast.multicast_publish_listener import MulticastPublisherListener
from model.inet.multicast.multicast_publisher import MulticastPublisher
from model.inet.multicast.multicast_receiver import MulticastReceiver
from model.inet.multicast.multicast_receiver_listener import MulticastReceiverListener
from model.inet.ping.ping import Ping
from model.inet.ping.ping_listener import PingListener
from model.inet.unicast.unicast_receiver import UnicastReceiver
from model.inet.unicast.unicast_receiver_listener import UnicastReceiverListener
from model.inet.unicast.unicast_sender import UnicastSender
from model.messages.announcement_message import AnnouncementMsg, make_key_for_announcement_msg
from model.messages.message_acceptor import MessageAcceptor
from model.messages.message_acceptor_listener import MessageAcceptorListener
from model.messages.message_builder import MessageBuilder
from model.players.inet_for_players import InetForPlayers


class InetController(MulticastPublisherListener, MulticastReceiverListener, UnicastReceiverListener,
                     MessageAcceptorListener, PingListener, InetForPlayers):
    def __init__(self, listener, port, ping_delay_ms, node_time_out_ms):
        self.listener = listener
        self.players = None
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.bind(('', port))
        print(f"Socket port {port}")

        self.message_acceptor = MessageAcceptor(self)
        self.ping = Ping(self, ping_delay_ms, node_time_out_ms)
        self.ping.start()
        self.sender = UnicastSender(self.socket, self.message_acceptor, self.ping, ping_delay_ms)
        self.receiver = UnicastReceiver(self, self.socket, self.message_acceptor)
        self.receiver.start()

        self.message_sequence = 0
        self.announcement_msg_map = OrderedDict()
        self.lock = Lock()  # Create a lock for synchronization
        self.invite_receiver = None  # Initialize to None
        self.invite_sender = None  # Initialize to None
        self.start_multicast_receiver()

    def interrupt_unicast(self):
        self.ping.stop_thread()
        self.receiver.stop()

    def attach_players(self, players):
        self.players = players

    def start_multicast_receiver(self):
        self.invite_receiver = MulticastReceiver(self)
        self.announcement_msg_map.clear()
        self.invite_receiver.start()

    def stop_multicast_receiver(self):
        if self.invite_receiver is not None:
            self.invite_receiver.stop()

    def start_multicast_publisher(self, node_id, config, players):
        self.invite_sender = MulticastPublisher(self, node_id, config, players)
        self.invite_sender.start()

    def stop_multicast_publisher(self):
        if self.invite_sender is not None:
            self.invite_sender.stop()

    def get_message_sequence(self):
        with self.lock:
            self.message_sequence += 1
            return self.message_sequence

    def get_announcement_msg(self, key):
        return self.announcement_msg_map.get(key)

    def send_ack_message(self, player, message):
        self.sender.send_message(player, message)

    def set_time_of_received_message(self, player_id):
        self.ping.set_time_of_received_message(player_id)

    def send_message(self, player, message):
        self.sender.send_message(player, MessageBuilder.set_message_sequence(message, self.get_message_sequence()))

    def launch_game_core(self, player_id):
        self.listener.launch_game_core(player_id)

    def get_game_player_by_id(self, player_id):
        return self.players.get_game_player_by_id(player_id)

    def receive_announcement_msg(self, msg, master_ip):
        try:
            master = AnnouncementMsg.get_master_player_from_game_message(msg)
            master.ip = master_ip
            with self.lock:
                key = make_key_for_announcement_msg(master.ip_address, master.port)
                if key not in self.announcement_msg_map:
                    self.announcement_msg_map[key] = AnnouncementMsg(msg, master)
                else:
                    self.announcement_msg_map[key].update_time()
                games_list = []
                for announcement_msg in list(self.announcement_msg_map.values()):
                    if not announcement_msg.is_actual():
                        del self.announcement_msg_map[announcement_msg.get_key_for_map()]
                    else:
                        if announcement_msg.game_message.games[0].can_join:
                            games_list.append(
                                f"{announcement_msg.get_key_for_map()}:"
                                f"{len(announcement_msg.game_message.games[0].players.players)}")
                self.listener.update_find_game_list(games_list)
        except AttributeError as e:
            print(traceback.format_exc(), 'cont 113')

    def receive_game_state_msg(self, game_state, master_ip):
        self.listener.receive_game_state_msg(game_state, master_ip)

    def receive_role_change_msg(self, role_change_msg, sender_id):
        self.listener.receive_role_change_msg(role_change_msg, sender_id)

    def receive_steer_msg(self, direction, sender_id):
        self.listener.receive_steer_msg(direction, sender_id)

    def receive_join_msg(self, name, ip, port, role, player_type):
        return self.listener.receive_join_msg(name, ip, port, role, player_type)

    def disconnect_player(self, player_id):
        self.players.disconnect_player(player_id)

    def send_ping(self, player_id):
        self.players.send_ping(player_id)

    def remove_player_from_ping(self, player_id):
        self.ping.remove_player(player_id)

    def get_opportunity_to_join(self):
        return self.listener.get_opportunity_to_join()
