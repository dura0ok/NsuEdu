from abc import abstractmethod, ABC


class PlayersForActionUpdater(ABC):
    @abstractmethod
    def initiate_deputy_player(self):
        pass

    @abstractmethod
    def transfer_role_of_the_master(self):
        pass

    @abstractmethod
    def send_game_state_to_all_players(self):
        pass
