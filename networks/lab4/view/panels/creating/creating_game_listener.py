from abc import ABC, abstractmethod

class CreatingGameListener(ABC):
    @abstractmethod
    def start_the_game(self):
        pass

    @abstractmethod
    def close_the_game(self):
        pass

    @abstractmethod
    def back_to_start_menu(self):
        pass

    @abstractmethod
    def open_config_settings(self):
        pass
