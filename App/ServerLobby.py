from Tkinter import *

class ServerLobby(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent)
        self.parent = parent
        self.initUI()
        self.parent.client.start(self.onReady, self.onPlayerUpdates, None, None)
        self.pack(anchor='c', expand=True)

    def initUI(self):
        self.status_text = Label(self, text='Waiting for players to ready up...')
        self.ready_button = Button(self, text='Toggle Ready', command=self.ready)
        self.status_text.grid(row=0, column=0)
        self.ready_button.grid(row=1, column=0)
        pass

    def ready(self):
        self.parent.client.toggle_ready()

    def onPlayerUpdates(self, updates):
        pass

    def onReady(self, countdown):
        self.status_text['text'] = "Starting game in {0}...".format(countdown)
        if countdown == 0:
            self.parent.goToGame()
