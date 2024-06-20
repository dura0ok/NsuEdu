import selectors
import socket
import sys

from State import State
from config import Config


def handle_greeting_state(sock: socket.socket, data):
    greeting = sock.recv(2)
    num_auth_support: int = greeting[1]
    methods = sock.recv(num_auth_support)

    if b'\x00' not in methods:
        raise Exception("Not auth method not supported")

    handshake_bytes = bytes([5, 0])
    sock.sendall(handshake_bytes)
    data["state"] = State.CONNECTION_REQUEST


def handle_connection_request(sock: socket.socket, data):
    header_data = sock.recv(4)
    if header_data[0] != 0x05:
        raise Exception("version not support")

    if header_data[1] != 0x01:
        raise Exception("tcp established only support")

    if header_data[2] != 0x00:
        raise Exception("must be 00")

    host_type = header_data[3]
    sizes_depend_types = {0x01: 4, 0x03: 1, 0x04: 16}
    if host_type == 0x03:
        sizes_depend_types[0x03] = int.from_bytes(sock.recv(1), byteorder='big')

    raw_host = sock.recv(sizes_depend_types[host_type])
    raw_port = sock.recv(2)
    host = socket.inet_ntoa(raw_host) if host_type != 0x03 else raw_host.decode()
    port = int.from_bytes(raw_port, byteorder='big')
    print(host, port)
    ip_address = socket.gethostbyname(socket.gethostname())
    b = bytes([0x05, 0x00, 0x00, 0x01]) + socket.inet_pton(socket.AF_INET, ip_address) + port.to_bytes(2, 'big')
    sock.sendall(b)

    data["state"] = State.ALIVE_STATE
    data["target"] = (host, port)
    # handle_alive(sock, data)


def forward_data(source_socket, target_host, target_port):
    target_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    target_socket.connect((target_host, target_port))

    while True:
        try:
            input_query = source_socket.recv(1024 * 4)
            target_socket.sendall(input_query)
            print("recv")   
            output_query = target_socket.recv(1024 * 4)
            source_socket.sendall(output_query)
            print("send")
        except Exception as e:
            print(e)
            break

    target_socket.close()
    source_socket.close()


def handle_alive(key: selectors.SelectorKey, selector: selectors.BaseSelector):
    data: dict = key.data
    if not isinstance(key.fileobj, socket.socket):
        raise Exception("something went wrong")
    sock: socket.socket = key.fileobj
    host, port = data["target"]
    forward_data(sock, host, port)
    selector.unregister(sock)


def handle_client(key: selectors.SelectorKey, selector: selectors.BaseSelector):
    handlers = {
        State.GREETING: handle_greeting_state,
        State.CONNECTION_REQUEST: handle_connection_request,
        State.ALIVE_STATE: handle_alive}
    if not isinstance(key.fileobj, socket.socket):
        raise Exception("something went wrong")
    sock: socket.socket = key.fileobj
    data: dict = key.data
    try:
        handlers[data["state"]](sock, data)
    except socket.error as e:
        print(e, file=sys.stderr)
        sock.close()
    except Exception as e:
        print(e, file=sys.stderr)


def handle_new_connection(key: selectors.SelectorKey, selector: selectors.BaseSelector):
    if not isinstance(key.fileobj, socket.socket):
        raise Exception("something went wrong")
    sock: socket.socket = key.fileobj

    conn, addr = sock.accept()
    print('accepted', conn, 'from', addr)
    conn.setblocking(False)
    selector.register(conn, selectors.EVENT_READ, {'handler': handle_client, 'state': State.GREETING})


def run_server(config: Config) -> None:
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.setblocking(False)
    server_socket.bind((config.host, config.port))
    server_socket.listen()
    selector = selectors.DefaultSelector()
    selector.register(server_socket, selectors.EVENT_READ, {'handler': handle_new_connection, 'state': None})

    while True:
        events = selector.select()
        for key, mask in events:
            callback = key.data['handler']
            if key.data["state"] == State.ALIVE_STATE and key.fileobj != server_socket.fileno():
                callback = handle_alive
            callback(key, selector)
