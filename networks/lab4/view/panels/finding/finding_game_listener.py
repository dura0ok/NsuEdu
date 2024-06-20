from abc import ABC, abstractmethod


class FindingGameListener(ABC):

    def launch_finding_game_module(self):
        pass


    def close_the_game(self):
        pass


    def back_to_start_menu(self):
        pass
