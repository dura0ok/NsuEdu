from abc import ABC, abstractmethod


class GamePanelListener(ABC):
    @abstractmethod
    def change_role_on_viewer(self):
        pass

    @abstractmethod
    def leave_the_game(self):
        pass
