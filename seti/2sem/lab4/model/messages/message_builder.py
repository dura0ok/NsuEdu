from lib import GameMessage, GameMessagePingMsg, GameMessageSteerMsg, GameMessageAckMsg, GameMessageStateMsg, \
    GameMessageAnnouncementMsg, GameAnnouncement, GameMessageJoinMsg, GameMessageErrorMsg, GameMessageRoleChangeMsg, \
    GamePlayers, GameConfig


class MessageBuilder:
    @staticmethod
    def ping_msg_builder(sender_id, receiver_id):
        return GameMessage(
            ping=GameMessagePingMsg(),
            msg_seq=0,
            sender_id=sender_id,
            receiver_id=receiver_id
        )

    @staticmethod
    def steer_msg_builder(direction, sender_id, receiver_id):
        return GameMessage(
            steer=GameMessageSteerMsg(
                direction=direction
            ),
            msg_seq=0,
            sender_id=sender_id,
            receiver_id=receiver_id
        )

    @staticmethod
    def ack_msg_builder(message_sequence, sender_id, receiver_id):
        return GameMessage(
            ack=GameMessageAckMsg(
                seq=message_sequence,
                sender_id=sender_id,
                player_id=receiver_id
            ))

    @staticmethod
    def state_msg_builder(game_state, sender_id, receiver_id):
        return GameMessage(
            state=GameMessageStateMsg(
                state=game_state
            ),
            msg_seq=0,
            sender_id=sender_id,
            receiver_id=receiver_id
        )

    @staticmethod
    def announcement_msg_builder(players: GamePlayers, config: GameConfig, can_join: bool, sender_id: int):
        return GameMessage(
            announcement=GameMessageAnnouncementMsg(
                games=[GameAnnouncement(players=players,
                                        config=config,
                                        can_join=can_join)]

            ),
            msg_seq=0,
            sender_id=sender_id
        )

    @staticmethod
    def join_msg_builder(player_type, name, sender_id, requested_role, receiver_id):
        return GameMessage(
            join=GameMessageJoinMsg(
                player_name=name,
                game_name=name,
                player_type=player_type,
                requested_role=requested_role
            ),
            msg_seq=0
        )

    @staticmethod
    def error_msg_builder(error_message, sender_id, receiver_id):
        return GameMessage(
            error=GameMessageErrorMsg(
                error_message=error_message
            ),
            msg_seq=0,
            sender_id=sender_id,
            receiver_id=receiver_id
        )

    @staticmethod
    def role_changing_msg_builder(sender_role, receiver_role, sender_id, receiver_id):
        return GameMessage(
            role_change=GameMessageRoleChangeMsg(
                sender_role=sender_role,
                receiver_role=receiver_role
            ),
            msg_seq=0,
            sender_id=sender_id,
            receiver_id=receiver_id
        )

    @staticmethod
    def set_message_sequence(game_message: GameMessage, message_seq):
        game_message.msg_seq = message_seq
        return game_message


def convert_keys_to_snake_case(dictionary):
    """
    Convert all keys in a dictionary to snake case.
    """
    new_dict = {}
    for key, value in dictionary.items():
        # Convert key to snake case
        snake_case_key = ''.join(['_' + i.lower() if i.isupper() else i for i in key]).lstrip('_')
        new_dict[snake_case_key] = value
    return new_dict
