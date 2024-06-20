import ipaddress
import socket
import threading
import time
import traceback

import betterproto

from lib import GamePlayer, GameMessage
from model.inet.unicast.acceptor_for_sender import AcceptorForSender
from model.inet.unicast.ping_for_sender import PingForSender
import sys
from lib.snakes import *


class UnicastSender:
    def __init__(self, sock: socket, message_acceptor: AcceptorForSender, ping: PingForSender, ping_delay: int):
        self.socket = sock
        self.message_acceptor = message_acceptor
        self.ping = ping
        self.ping_delay = ping_delay

    def send_message(self, player: GamePlayer, message: GameMessage) -> None:
        sender_schedule = SenderSchedule(self.socket, self.message_acceptor, self.ping, self.ping_delay, player,
                                         message)
        sender_schedule.start()


class SenderSchedule(threading.Thread):
    def __init__(self, sock: socket, message_acceptor: AcceptorForSender, ping: PingForSender,
                 ping_delay: int, player: GamePlayer, message: GameMessage):
        super().__init__()
        self.socket = sock
        self.message_acceptor = message_acceptor
        self.ping = ping
        self.message = message
        self.player = player
        self.ping_delay = ping_delay

    def send(self) -> None:
        print(
            f"Unicast Sender is sending a {betterproto.which_one_of(self.message, 'Type')}"
            f" message seq = {self.message.msg_seq} to id={self.player.id} "
            f"ip:port={self.player.ip_address}:{self.player.port}")
        buffer = bytes(self.message)
        ip = ipaddress.IPv4Address(self.player.ip_address).exploded
        packet = (buffer, (ip, self.player.port))
        self.socket.sendto(*packet)

    def run(self) -> None:
        self.ping.set_time_of_sent_message(self.player.id)
        if betterproto.which_one_of(self.message, 'Type')[0] == "Ack":
            try:
                self.send()
            except Exception as e:
                print(e, 'unicast sender 54')
                time.sleep(1)
        else:
            while True:
                try:
                    self.send()
                    time.sleep(self.ping_delay)
                    if self.message_acceptor.check_accepted_message(self.message.msg_seq):
                        break
                    if not self.ping.is_alive_player(self.player.id):
                        print(f"Sender out seq = {self.message.msg_seq} because id={self.player.id} is not available")
                        break
                except Exception as e:
                    print(traceback.format_exc(), 'unicast sender', self.message)
                    time.sleep(1)
