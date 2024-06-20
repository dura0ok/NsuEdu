from datetime import datetime

from lib import NodeRole


def make_key_for_announcement_msg(*args):
    return ":".join(map(str, args))


class AnnouncementMsg:
    def __init__(self, game_message, master_player):
        now = datetime.now()
        self.game_message = game_message
        self.time_of_last_received_msg = now.timestamp()
        self.master = master_player

    def update_time(self):
        now = datetime.now()
        self.time_of_last_received_msg = now.timestamp()

    def is_actual(self):
        now = datetime.now()
        return now.timestamp() - self.time_of_last_received_msg < 6000

    @staticmethod
    def get_master_player_from_game_message(game_message):
        for player in game_message.games[0].players.players:
            if player.role == NodeRole.MASTER:
                return player
        return None

    def get_key_for_map(self):
        return make_key_for_announcement_msg(self.master.ip_address, self.master.port)
