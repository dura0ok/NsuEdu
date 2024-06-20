import ipaddress
import threading
import traceback
from socket import socket
from typing import Union

import betterproto

from lib import GameMessage, NodeRole
from model.inet.unicast.acceptor_for_receiver import AcceptorForReceiver
from model.inet.unicast.unicast_receiver_listener import UnicastReceiverListener


class UnicastReceiver(threading.Thread):
    def __init__(self, listener: UnicastReceiverListener, sock: socket, message_acceptor: AcceptorForReceiver):
        super().__init__()
        self.listener = listener
        self.socket = sock
        self.message_acceptor = message_acceptor
        self._stop_event = threading.Event()

    def stop(self):
        print('im has been stoped(')
        self._stop_event.set()

    def stopped(self):
        return self._stop_event.is_set()

    def message_type_handler(self, msg: GameMessage, address: Union[bytes, str], port: int) -> None:
        message_sender_id = msg.sender_id
        message_sequence = msg.msg_seq
        msg_type_case = betterproto.which_one_of(msg, "Type")[0]
        print(msg, msg_type_case, port)

        if msg_type_case == 'ping':
            print(f"PING id: {message_sender_id} seq= {message_sequence}")
            self.message_acceptor.accept_message(message_sender_id, message_sequence)
        elif msg_type_case == 'steer':
            print(f"STEER id: {message_sender_id} seq= {message_sequence}")
            is_message_from_known_player = self.message_acceptor.accept_message(message_sender_id, message_sequence)
            if is_message_from_known_player:
                self.listener.receive_steer_msg(msg.steer.direction, message_sender_id)
        elif msg_type_case == 'ack':
            print(f"ACK id: {message_sender_id} seq= {message_sequence}")
            self.message_acceptor.receive_ack_msg(msg.ack.player_id, msg.ack.sender_id, msg.ack.seq)
        elif msg_type_case == 'state':
            print(f"STATE id: {message_sender_id} seq= {message_sequence}")
            if self.listener.listener.players.node_id == 1: exit()
            self.listener.receive_game_state_msg(msg.state.state, ipaddress.IPv4Address(address).exploded)
        elif msg_type_case == 'join':
            print(f"JOIN id: {message_sender_id} seq= {message_sequence}")
            join_msg = msg.join
            new_player_id = self.listener.receive_join_msg(join_msg.player_name,
                                                           ipaddress.IPv4Address(address).exploded, port,
                                                           NodeRole.VIEWER if join_msg.requested_role
                                                           else NodeRole.NORMAL,
                                                           join_msg.player_type)
            self.message_acceptor.accept_message(new_player_id, message_sequence)
        elif msg_type_case == 'error':
            print(f"ERROR id: {message_sender_id} seq= {message_sequence}")
            self.message_acceptor.accept_message(message_sender_id, message_sequence)
        elif msg_type_case == 'role_change':
            print(
                f"ROLE_CHANGE from id: {message_sender_id} ({msg.role_change.sender_role}) to "
                f"{msg.role_change.receiver_role} seq= {message_sequence}")
            is_message_from_known_player = self.message_acceptor.accept_message(message_sender_id, message_sequence)
            if is_message_from_known_player:
                self.listener.receive_role_change_msg(msg.role_change, message_sender_id)

    def run(self) -> None:
        try:
            print("Unicast Receiver started")
            while self.is_alive():
                received_unknown_message_data, (sender_address, port) = self.socket.recvfrom(1024)
                print('recived', received_unknown_message_data)
                received_unknown_message = GameMessage().parse(received_unknown_message_data)
                self.message_type_handler(received_unknown_message, sender_address, port)

        except Exception as e:
            print(f"Exception in Unicast Receiver: {traceback.format_exc()}")
        finally:
            print("Unicast Receiver finished")
