import tkinter as tk
import numpy as np
import socket
from tkinter import colorchooser


led_data = np.zeros(shape=64*3,dtype=np.uint8)

server_ip = "192.168.0.109"  # Replace with your ESP32's IP
server_port = 12345

# Create a TC{} socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((server_ip, server_port))
client_socket.settimeout(20)

def choose_color():
    """Choose a color to be applied to grid cells."""
    global selected_color
    color = colorchooser.askcolor()  # Get the hex color code
    if color:
        selected_color = color
        color_display.config(bg=selected_color[1])

def apply_color(row, col):
    """Apply the selected color to a grid cell."""
    if selected_color:
        stride0 = 24
        stride1 = 3

        buttons[row][col].config(bg=selected_color[1])
        led_data[row*stride0+col*stride1+0]=selected_color[0][0]
        led_data[row*stride0+col*stride1+1]=selected_color[0][1]
        led_data[row*stride0+col*stride1+2]=selected_color[0][2]


# Placeholder functions for additional buttons
def function_publish_display():
            
    client_socket.sendall(led_data.tobytes())
    # Receive the response from the server
    data, addr = client_socket.recvfrom(1024)
    print(f"Response from server: {data.decode()}")
    print("Artwork Published!")

def function_clear():
    global selected_color
    selected_color = ((0,0,0),"black")
    for row in range(8):
        for col in range(8):
            apply_color(row, col)
    print("Cleared")

def function_canvas():
    for row in range(8):
        for col in range(8):
            apply_color(row, col)
    print("Canvas")

def function_4():
    print("Function 4 executed!")

# Initialize the main window
root = tk.Tk()
root.title("8x8 Color Grid")

# Default selected color
selected_color = ((0,0,0),"black")

# Create a frame for the grid and controls
grid_frame = tk.Frame(root)
grid_frame.pack(side=tk.LEFT, padx=10, pady=10)

control_frame = tk.Frame(root)
control_frame.pack(side=tk.RIGHT, padx=10, pady=10)

# Create the 8x8 grid of buttons
buttons = []
for row in range(8):
    button_row = []
    for col in range(8):
        button = tk.Button(
            grid_frame, 
            bg=selected_color[1], 
            width=6, 
            height=3, 
            command=lambda r=row, c=col: apply_color(r, c)
        )
        button.grid(row=row, column=col, padx=1, pady=1)
        button_row.append(button)
    buttons.append(button_row)

# Create controls for selecting and displaying the current color
color_display = tk.Label(control_frame, text="Selected Color", bg=selected_color[1], width=15, height=2)
color_display.pack(pady=10)

color_button = tk.Button(control_frame, text="Choose Color", command=choose_color, width=15)
color_button.pack(pady=10)

# Additional buttons with placeholder functions
button1 = tk.Button(control_frame, text="Send To display", command=function_publish_display, width=15)
button1.pack(pady=5)

button2 = tk.Button(control_frame, text="Clear!", command=function_clear, width=15)
button2.pack(pady=5)

button3 = tk.Button(control_frame, text="Canvas", command=function_canvas, width=15)
button3.pack(pady=5)

button4 = tk.Button(control_frame, text="Button 4", command=function_4, width=15)
button4.pack(pady=5)

# Start the Tkinter event loop
root.mainloop()
