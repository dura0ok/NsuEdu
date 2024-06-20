import argparse
import signal
import socket
import struct

msg = "alive".encode()


def send_multicast_message(server_address: tuple):
    sock = socket.socket(socket.AF_INET6 if ":" in server_address[0] else socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.IPPROTO_IPV6 if ":" in server_address[0] else socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
    sock.sendto(msg, server_address)
    sock.close()


def receive_multicast_messages(sock):
    live_copies = []

    while True:
        try:
            data, address = sock.recvfrom(1024)
            print(address)
            live_copies.append(address[0])
        except socket.timeout:
            break

    return live_copies  # Return the list of IP addresses of live copies


def main(server_address):
    print(server_address)
    sock = socket.socket(socket.AF_INET6 if ":" in server_address[0] else socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('', server_address[1]))

    def handle_exit(sig, frame):
        print("\nExiting...")
        sock.close()
        exit(0)

    signal.signal(signal.SIGINT, handle_exit)

    if ":" in server_address[0]:
        # For IPv6
        group_bin = socket.inet_pton(socket.AF_INET6, server_address[0])
        mreq = group_bin + struct.pack("@I", 0)
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, mreq)
    else:
        # For IPv4
        group_bin = socket.inet_aton(server_address[0])
        mreq = group_bin + struct.pack("=I", socket.INADDR_ANY)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    while True:
        send_multicast_message(server_address)
        live_copies = receive_multicast_messages(sock)
        print("Live copies:", live_copies)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="239.255.255.254", help="IP address of the server")
    parser.add_argument("--port", type=int, default=4446, help="Port number")

    args = parser.parse_args()

    main((args.host, args.port))
