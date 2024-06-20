from abc import ABC, abstractmethod

class ConfigurationSettingsListener(ABC):
    @abstractmethod
    def close_the_game(self):
        pass

    @abstractmethod
    def back_to_creating_game_menu(self):
        pass
