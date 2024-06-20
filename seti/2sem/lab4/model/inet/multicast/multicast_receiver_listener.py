from abc import ABC, abstractmethod

from lib import GameMessageAnnouncementMsg


class MulticastReceiverListener(ABC):
    @abstractmethod
    def receive_announcement_msg(self, msg: GameMessageAnnouncementMsg, master_ip: str):
        pass
