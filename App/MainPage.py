from Tkinter import *
from IPInput import *
from tkMessageBox import *
from subprocess import call

class MainPage(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent)
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
        if self.startServer():
            self.joinServer("localhost")

    def startServer(self):
        try:
            call(["../Server/server", ""])
            return True
        except:
            showerror(message="Server has not been compiled, or could not be found in ../Server")
            return False

    def onJoinServerPressed(self):
        self.ip_input_window = Toplevel(self.parent)
        self.ip_input = IPInput(self)
        self.parent.parent.eval('tk::PlaceWindow %s center' % self.parent.parent.winfo_pathname(self.ip_input_window.winfo_id()) )

    def joinServer(self, ip):
        self.parent.goToServerLobby(ip)