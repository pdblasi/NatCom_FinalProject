from Tkinter import *
from MainPage import *
from Game import *
from ServerLobby import *
from ThreadSafeFrame import *
from Client import Client

class App(ThreadSafeFrame):
    def __init__(self, parent):
        ThreadSafeFrame.__init__(self, parent)
        self.parent = parent
        self.parent.title("eVolved")
        self.content = MainPage(self)
        self.pack(anchor='c', expand=True)

    def goToServerLobby(self, ip):
        self.run_on_UI_thread(lambda: self.__replace_with_lobby(ip))

    def goToGame(self):
        self.run_on_UI_thread(lambda: self.__replace_with_game())

    def __replace_with_lobby(self, ip):
        self.content.destroy()
        self.client = Client.Client(ip)
        self.content = ServerLobby(self)
        self.pack(anchor='c', expand=True)

    def __replace_with_game(self):
        self.content.destroy()
        self.content = Game(self)