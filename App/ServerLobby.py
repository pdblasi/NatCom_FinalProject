from Tkinter import *

class ServerLobby(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent)
        self.parent = parent
        self.initUI()
        self.pack(anchor='c', expand=True)

    def initUI(self):
        Label(self, text="WE'RE IN THE LOBBY!").grid()