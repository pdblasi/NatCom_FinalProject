from Tkinter import *
from tkMessageBox import *
from subprocess import call

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

class ServerLobby(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent)
        self.parent = parent
        self.initUI()
        self.pack(anchor='c', expand=True)

    def initUI(self):
        Label(self, text="WE'RE IN THE LOBBY!").grid()

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
        self.startServer()
        self.joinServer("localhost")

    def startServer(self):
        try:
            call(["../Server/server", ""])
        except:
            showerror(message="Server has not been compiled, or could not be found in ../Server")

    def onJoinServerPressed(self):
        self.ip_input_window = Toplevel(self.parent)
        self.ip_input = IPInput(self)
        self.parent.parent.eval('tk::PlaceWindow %s center' % self.parent.parent.winfo_pathname(self.ip_input_window.winfo_id()) )

    def joinServer(self, ip):
        self.parent.goToServerLobby(ip)



class IPInput(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent.ip_input_window)
        self.parent = parent
        self.initUI()
        self.pack()

    def initUI(self):
        self.ip_input = Entry(self)
        self.port_label = Label(self, text=":35015")
        self.join_button = Button(self, text="Join", command=self.joinServer)

        self.ip_input.grid(row=0, column=0)
        self.port_label.grid(row=0, column=1)
        self.join_button.grid(row=1, columnspan=2)

    def joinServer(self):
        self.parent.joinServer(self.ip_input.get())
        self.parent.ip_input_window.destroy()
                

def main():
    root = Tk()
    root.geometry("500x500")
    root.resizable(width=False, height=False)
    app = App(root)
    root.mainloop()  


if __name__ == '__main__':
    main() 