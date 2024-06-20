import threading
import time
from collections import defaultdict
from datetime import datetime


class PingSender(threading.Thread):
    def __init__(self, listener, ping_delay):
        super().__init__()
        self.listener = listener
        self.players_ping_time = defaultdict(int)
        self.ping_delay = ping_delay
        self._stop_event = threading.Event()

    def remove_player(self, player_id):
        with self._lock:
            del self.players_ping_time[player_id]

    def set_player_ping_time(self, player_id):
        self.players_ping_time[player_id] = int(datetime.now().timestamp() * 1000)

    def run(self):
        try:
            delay = max(self.ping_delay // 4, 1)
            while not self._stop_event.is_set():
                for player_id, ping_time in list(self.players_ping_time.items()):
                    now = int(datetime.now().timestamp() * 1000)
                    if now - ping_time > self.ping_delay:
                        self.listener.send_ping(player_id)
                time.sleep(delay)
        except KeyboardInterrupt as e:
            print(f"Ping Sender: {e}")
        finally:
            with self._lock:
                self.players_ping_time.clear()
            print("Ping Sender finished")

    def stop(self):
        self._stop_event.set()

    def __enter__(self):
        self._lock = threading.Lock()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.stop()
