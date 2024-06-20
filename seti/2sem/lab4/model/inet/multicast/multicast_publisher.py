import socket
import struct
import time
from threading import Thread
from typing import List

from lib import GamePlayer, GameConfig, GamePlayers
from model.inet.multicast.multicast_publish_listener import MulticastPublisherListener
from model.messages.message_builder import MessageBuilder


class MulticastPublisher(Thread):
    def __init__(self, listener: MulticastPublisherListener, node_id: int, config: GameConfig,
                 players: List[GamePlayer]) -> None:
        super().__init__()
        self.listener = listener
        self.node_id = node_id
        self.config = config
        self.players_list = players
        self.socket: socket.socket | None = None
        self._is_stopped = False  # Add this line

    def get_message_bytes(self) -> bytes:
        message = MessageBuilder.announcement_msg_builder(
            GamePlayers(players=self.players_list), self.config, self.listener.get_opportunity_to_join(), self.node_id)
        MessageBuilder.set_message_sequence(message, self.listener.get_message_sequence())
        return message.SerializeToString()

    def run(self) -> None:
        try:
            print("Multicast publisher started")
            group = "239.192.0.4"
            port = 9192
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            self.socket.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
            while not self._is_stopped:  # Use _is_stopped instead of is_interrupted()
                message_buffer = self.get_message_bytes()
                self.socket.sendto(message_buffer, (group, port))
                time.sleep(1)
        except KeyboardInterrupt:
            pass
        finally:
            if self.socket is not None:
                self.socket.close()
            print("Multicast publisher finished")

    def stop(self) -> None:
        self._is_stopped = True
