from Tkinter import *
from IPInput import *
from tkMessageBox import *
from subprocess import call
from ThreadSafeFrame import *
import thread

class MainPage(ThreadSafeFrame):
    def __init__(self, parent):
        ThreadSafeFrame.__init__(self, parent)
        self.parent = parent
        self.initUI()
        self.pack(anchor='c', expand=True)
    
    def initUI(self):
        self.title_label = Label(self, text="eVolved", font=("Helvetica", 64))
        self.start_game_button = Button(self, text="Start & Join Server", command=self.startGame)
        self.start_server_button = Button(self, text="Start Server", command=self.startServer)
        self.join_server_button = Button(self, text="Join Server", command=self.onJoinServerPressed)

        self.title_label.grid(row=0, pady=30)
        self.start_game_button.grid(row=1, pady=10, sticky="we")
        self.start_server_button.grid(row=2, pady = 10, sticky="we")
        self.join_server_button.grid(row=3, pady=10, sticky="we")

    def startGame(self):
        self.startServer(lambda: self.joinServer("localhost"))

    def startServer(self, onSuccess):
        thread.start_new_thread(self.__startServer, ())
        onSuccess()

    def __startServer(self):
        try:
            call(["./Server/server", ""])
        except:
            self.run_on_UI_thread(lambda: showerror(message="Server has not been compiled, or could not be found in ./Server"))

    def onJoinServerPressed(self):
        self.ip_input_window = Toplevel(self.parent)
        self.ip_input = IPInput(self)

    def joinServer(self, ip):
        self.parent.goToServerLobby(ip)