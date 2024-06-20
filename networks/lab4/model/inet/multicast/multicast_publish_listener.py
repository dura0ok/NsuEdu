from abc import ABC, abstractmethod


class MulticastPublisherListener(ABC):
    @abstractmethod
    def get_message_sequence(self):
        pass

    @abstractmethod
    def get_opportunity_to_join(self):
        pass
