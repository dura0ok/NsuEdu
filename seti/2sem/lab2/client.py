import socket
import os


def send_file(server_address, server_port, file_path):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((server_address, server_port))

        # Отправка имени файла
        filename = os.path.basename(file_path)
        filename_length = len(filename).to_bytes(4, byteorder='big')
        client_socket.sendall(filename_length)
        client_socket.sendall(filename.encode())

        # Отправка размера файла
        file_size = os.path.getsize(file_path)
        file_size_bytes = file_size.to_bytes(8, byteorder='big')
        client_socket.sendall(file_size_bytes)

        # Отправка содержимого файла
        with open(file_path, 'rb') as file:
            while True:
                data = file.read(4096)
                if not data:
                    break
                client_socket.sendall(data)

        # Получение информации о результате операции от сервера
        success = client_socket.recv(1024).decode()
        if success == 'True':
            print("File transfer successful.")
        else:
            print("File transfer failed.")


if __name__ == "__main__":
    SERVER_ADDRESS = 'localhost'  # Адрес сервера
    SERVER_PORT = 12345  # Порт сервера
    FILE_PATH = 'Windows.iso'
    send_file(SERVER_ADDRESS, SERVER_PORT, FILE_PATH)
