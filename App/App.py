from Tkinter import *
from MainPage import *
from ServerLobby import *

class App(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent)
        self.parent = parent
        self.parent.title("eVolved")
        self.content = MainPage(self)
        self.pack(anchor='c', expand=True)

    def goToServerLobby(self, ip):
        self.content.destroy()
        self.content = ServerLobby(self)
        self.pack(anchor='c', expand=True)