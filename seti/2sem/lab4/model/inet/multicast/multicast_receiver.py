import socket
import struct
import threading
import traceback

from lib import GameMessage

import inspect


class MulticastReceiver(threading.Thread):
    def __init__(self, listener):
        super().__init__()
        self.listener = listener
        self.socket = None
        self._is_stopping = False

    def run(self):
        ip = '239.192.0.4'
        port = 9192
        group = (ip, port)
        try:
            print("Multicast receiver started")
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind(('', port))
            mreq = struct.pack("4sl", socket.inet_aton(ip), socket.INADDR_ANY)
            self.socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
            while not self._is_interrupted():
                data, address = self.socket.recvfrom(1024)
                gotBytes = data[:len(data)]
                msg = GameMessage().parse(data=gotBytes)
                self.listener.receive_announcement_msg(msg.announcement, address[0])
            self.socket.setsockopt(socket.IPPROTO_IP, socket.IP_DROP_MEMBERSHIP, struct.pack('4sL', socket.inet_aton(ip), socket.INADDR_ANY))
        except socket.error as e:
            print("Error: ", traceback.format_exc())
        finally:
            if self.socket is not None:
                self.socket.close()
            print("Multicast receiver finished")

    def _is_interrupted(self):
        return self._is_stopping or not self.is_alive()

    def stop(self):
        self._is_stopping = True

    def stopped(self):
        return self._is_stopping

    def stop_thread(self):
        print('stoped')
        self.stop()
        self.join()

# class MulticastReceiver(threading.Thread):
#     def __init__(self, listener):
#         super().__init__()
#         self.listener = listener
#         self.socket = None
#
#     def run(self):
#         ip = '239.192.0.4'
#         port = 9192
#         group = (ip, port)
#         try:
#             print("Multicast receiver started")
#             self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#             self.socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, struct.pack('4sL', socket.inet_aton(ip), socket.INADDR_ANY))
#             self.socket.bind(('', port))
#             while self._is_interrupted():
#                 data, address = self.socket.recvfrom(256)
#                 gotBytes = data[:len(data)]
#                 msg = GameMessage().parse(data=gotBytes)
#                 self.listener.receiveAnnouncementMsg(msg.announcement, address[0])
#             self.socket.setsockopt(socket.IPPROTO_IP, socket.IP_DROP_MEMBERSHIP, struct.pack('4sL', socket.inet_aton(ip), socket.INADDR_ANY))
#         except socket.error as e:
#             print("Error: ", e)
#         finally:
#             if self.socket is not None:
#                 self.socket.close()
#             print("Multicast receiver finished")