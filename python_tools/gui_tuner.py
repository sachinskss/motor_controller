import tkinter as tk
from tkinter import ttk
import socket
import threading
import time
import json

class GUITuner:
    def __init__(self, master):
        self.master = master
        master.title("Motor Controller Tuner")

        self.socket = None
        self.connected = False
        self.host = "127.0.0.1"
        self.port = 65432

        self.create_widgets()
        self.connect_to_simulation()

    def create_widgets(self):
        # Connection Status
        self.status_label = ttk.Label(self.master, text="Status: Disconnected", foreground="red")
        self.status_label.grid(row=0, column=0, columnspan=2, pady=5)

        # PI Gains for Id
        ttk.Label(self.master, text="Id Kp:").grid(row=1, column=0, padx=5, pady=2, sticky="w")
        self.id_kp_var = tk.DoubleVar(value=0.5)
        ttk.Entry(self.master, textvariable=self.id_kp_var).grid(row=1, column=1, padx=5, pady=2)

        ttk.Label(self.master, text="Id Ki:").grid(row=2, column=0, padx=5, pady=2, sticky="w")
        self.id_ki_var = tk.DoubleVar(value=10.0)
        ttk.Entry(self.master, textvariable=self.id_ki_var).grid(row=2, column=1, padx=5, pady=2)

        # PI Gains for Iq
        ttk.Label(self.master, text="Iq Kp:").grid(row=3, column=0, padx=5, pady=2, sticky="w")
        self.iq_kp_var = tk.DoubleVar(value=0.5)
        ttk.Entry(self.master, textvariable=self.iq_kp_var).grid(row=3, column=1, padx=5, pady=2)

        ttk.Label(self.master, text="Iq Ki:").grid(row=4, column=0, padx=5, pady=2, sticky="w")
        self.iq_ki_var = tk.DoubleVar(value=10.0)
        ttk.Entry(self.master, textvariable=self.iq_ki_var).grid(row=4, column=1, padx=5, pady=2)

        # Target Currents
        ttk.Label(self.master, text="Target Id:").grid(row=5, column=0, padx=5, pady=2, sticky="w")
        self.target_id_var = tk.DoubleVar(value=0.0)
        ttk.Entry(self.master, textvariable=self.target_id_var).grid(row=5, column=1, padx=5, pady=2)

        ttk.Label(self.master, text="Target Iq:").grid(row=6, column=0, padx=5, pady=2, sticky="w")
        self.target_iq_var = tk.DoubleVar(value=1.0)
        ttk.Entry(self.master, textvariable=self.target_iq_var).grid(row=6, column=1, padx=5, pady=2)

        # Send Button
        self.send_button = ttk.Button(self.master, text="Send Gains & Targets", command=self.send_data)
        self.send_button.grid(row=7, column=0, columnspan=2, pady=10)

    def connect_to_simulation(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port)) # Connect to localhost on port 65432
            self.connected = True
            self.status_label.config(text="Status: Connected", foreground="green")
            print("Connected to simulation.")
            # Start a thread to receive data from the simulation (optional)
            # self.receive_thread = threading.Thread(target=self.receive_data_loop)
            # self.receive_thread.daemon = True
            # self.receive_thread.start()
        except ConnectionRefusedError:
            self.status_label.config(text="Status: Connection Refused", foreground="red")
            print("Connection refused. Is the simulation running and listening on port {}?".format(self.port))
        except Exception as e:
            self.status_label.config(text=f"Status: Error ({e})", foreground="red")
            print(f"Error connecting: {e}")

    def send_data(self):
        if not self.connected:
            print("Not connected to simulation.")
            return

        try:
            data = {
                "id_kp": self.id_kp_var.get(),
                "id_ki": self.id_ki_var.get(),
                "iq_kp": self.iq_kp_var.get(),
                "iq_ki": self.iq_ki_var.get(),
                "target_id": self.target_id_var.get(),
                "target_iq": self.target_iq_var.get(),
            }
            message = json.dumps(data).encode("utf-8")
            self.socket.sendall(message)
            print(f"Sent: {data}")
        except Exception as e:
            print(f"Error sending data: {e}")
            self.connected = False
            self.status_label.config(text="Status: Disconnected", foreground="red")

    def receive_data_loop(self):
        while self.connected:
            try:
                data = self.socket.recv(1024)
                if not data:
                    print("Simulation disconnected.")
                    self.connected = False
                    self.status_label.config(text="Status: Disconnected", foreground="red")
                    break
                print(f"Received: {data.decode("utf-8")}")
            except Exception as e:
                print(f"Error receiving data: {e}")
                self.connected = False
                self.status_label.config(text="Status: Disconnected", foreground="red")
                break
        self.socket.close()

    def on_closing(self):
        if self.connected:
            self.socket.close()
        self.master.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = GUITuner(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()
