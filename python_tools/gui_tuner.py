import tkinter as tk
from tkinter import Scale, Button, Label

class TunerApp:
    def __init__(self, root):
        self.root = root
        root.title("Motor Controller Tuner")
        Label(root, text="Speed PI - Kp").pack()
        self.kp_speed = Scale(root, from_=0, to=100, orient=tk.HORIZONTAL)
        self.kp_speed.pack()
        Label(root, text="Speed PI - Ki").pack()
        self.ki_speed = Scale(root, from_=0, to=100, orient=tk.HORIZONTAL)
        self.ki_speed.pack()
        Button(root, text="Apply", command=self.apply).pack()
    
    def apply(self):
        print(f"New gains: Kp={self.kp_speed.get()}, Ki={self.ki_speed.get()}")
        # In real usage, send over serial to simulation

if __name__ == '__main__':
    root = tk.Tk()
    app = TunerApp(root)
    root.mainloop()