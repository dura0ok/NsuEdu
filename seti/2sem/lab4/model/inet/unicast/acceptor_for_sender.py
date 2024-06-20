from abc import ABC, abstractmethod


class AcceptorForSender(ABC):
    @abstractmethod
    def check_accepted_message(self, seq_number):
        pass
