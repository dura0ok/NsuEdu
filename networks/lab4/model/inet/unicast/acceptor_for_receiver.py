from abc import ABC, abstractmethod


class AcceptorForReceiver(ABC):
    @abstractmethod
    def accept_message(self, player_id, message_sequence):
        pass

    @abstractmethod
    def receive_ack_msg(self, receiver_id, sender_id, message_sequence):
        pass
