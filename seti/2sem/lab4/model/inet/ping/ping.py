import threading
import time

from model.inet.ping.ping_sender import PingSender
from model.inet.unicast.ping_for_sender import PingForSender


class Ping(threading.Thread, PingForSender):
    def __init__(self, listener, ping_delay, node_timeout):
        super().__init__()
        self.listener = listener
        self.node_timeout = node_timeout
        self.last_message_time_from_node = {}
        self.ping_sender = PingSender(listener, ping_delay)
        self._is_stopping = False  # Added flag for stopping the thread

    def set_time_of_received_message(self, player_id):
        self.last_message_time_from_node[player_id] = int(time.time() * 1000)

    def set_time_of_sent_message(self, player_id):
        self.ping_sender.set_player_ping_time(player_id)

    def is_alive_player(self, player_id):
        return player_id in self.last_message_time_from_node

    def remove_player(self, player_id):
        del self.last_message_time_from_node[player_id]
        self.ping_sender.remove_player(player_id)
        print(f"Ping: delete dead node = {player_id}")

    def check_nodes(self):
        now = int(time.time() * 1000)
        for id_player in list(self.last_message_time_from_node.keys()):
            if now - self.last_message_time_from_node[id_player] > self.node_timeout:
                self.listener.disconnect_player(id_player)
                del self.last_message_time_from_node[id_player]
                self.ping_sender.remove_player(id_player)

    def run(self):
        delay = max(self.node_timeout // 4, 1)
        self.ping_sender.start()
        try:
            time.sleep(delay)
            while not self._is_stopping:  # Check the flag instead of is_interrupted()
                self.check_nodes()
                time.sleep(delay / 1000.0)
        except KeyboardInterrupt:
            pass
        finally:
            self.last_message_time_from_node.clear()
            print("Ping finished")
            self.ping_sender.stop()
            self.stop_thread()

    def stop_thread(self):
        self._is_stopping = True
