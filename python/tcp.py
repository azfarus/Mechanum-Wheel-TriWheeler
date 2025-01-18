import socket
import numpy as np
import time

server_ip = "192.168.0.106"  # Replace with your ESP32's IP
server_port = 12345

# Create a UDP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((server_ip, server_port))
client_socket.settimeout(20)
try:
    while True:
        # Send a message to the server
        message = np.random.randint(low=0, high=100, size=64, dtype=np.uint8).tobytes()
        print(message)
        if len(message) > 0:
            client_socket.sendall(message)
            
            # Receive the response from the server
            data, addr = client_socket.recvfrom(1024)
            print(f"Response from server: {data.decode()}")
            
        time.sleep(2)

finally:
    client_socket.close()
