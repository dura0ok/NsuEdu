import socket
import threading
import os
import time

UPLOADS_DIR = "uploads"


def handle_client(client_socket, client_address):
    try:
        # Получение имени файла от клиента
        filename_length = int.from_bytes(client_socket.recv(4), byteorder='big')
        filename = client_socket.recv(filename_length).decode()

        # Получение размера файла от клиента
        file_size = int.from_bytes(client_socket.recv(8), byteorder='big')

        # Подготовка пути сохранения файла
        file_path = os.path.join(UPLOADS_DIR, filename)

        # Создание директории uploads при необходимости
        os.makedirs(UPLOADS_DIR, exist_ok=True)

        # Прием содержимого файла от клиента и сохранение
        received_bytes = 0
        start_time = time.time()

        with open(file_path, 'wb') as file:
            while received_bytes < file_size:
                data = client_socket.recv(4096)
                if not data:
                    raise socket.error("closed client")
                file.write(data)
                received_bytes += len(data)

                # Вывод скорости приема
                elapsed_time = time.time() - start_time
                if elapsed_time >= 3:
                    average_speed = received_bytes / elapsed_time
                    current_speed = len(data) / elapsed_time

                    print(
                        f"Client {client_address}: Current Speed: {current_speed} bytes/sec, Average Speed: {average_speed} bytes/sec")

                    start_time = time.time()

        # Проверка корректности полученного файла
        success = file_size == received_bytes

        # Отправка клиенту информации о результате операции
        client_socket.sendall(str(success).encode())

        # Закрытие соединения с клиентом
        client_socket.close()
    except socket.error as e:
        print("closed client")


def main(port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', port))
    server_socket.listen(5)
    print(f"Server is listening on port {port}")

    while True:
        client_socket, client_address = server_socket.accept()
        # Запуск нового потока для обслуживания клиента
        thread = threading.Thread(target=handle_client, args=(client_socket, client_address))
        thread.start()


if __name__ == "__main__":
    PORT = 12345  # Порт сервера
    main(PORT)
