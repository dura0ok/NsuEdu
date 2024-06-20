from model.inet.unicast.acceptor_for_receiver import AcceptorForReceiver
from model.inet.unicast.acceptor_for_sender import AcceptorForSender
from model.messages.message_builder import MessageBuilder


class MessageAcceptor(AcceptorForSender, AcceptorForReceiver):
    def __init__(self, listener):
        self.listener = listener
        self.accepted_messages = set()
        self.this_node_id = 1

    def accept_message(self, player_id, message_sequence):
        self.listener.set_time_of_received_message(player_id)
        player = self.listener.get_game_player_by_id(player_id)
        if player:
            self.listener.send_ack_message(
                player, MessageBuilder.ack_msg_builder(message_sequence, self.this_node_id, player_id)
            )
            print("Message acceptor sent ACK to", player_id, 'seq =', message_sequence)
            return True
        return False

    def check_accepted_message(self, seq_number):
        return seq_number in self.accepted_messages

    def receive_ack_msg(self, receiver_id, sender_id, message_sequence):
        self.listener.set_time_of_received_message(sender_id)
        self.accepted_messages.add(message_sequence)

        if message_sequence == 1:
            self.this_node_id = receiver_id
            self.listener.launch_game_core(receiver_id)
