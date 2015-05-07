from Tkinter import *
from tkMessageBox import *
from ThreadSafeFrame import *

class Game(ThreadSafeFrame):
    SIZE = 500

    def __init__(self, parent):
        ThreadSafeFrame.__init__(self, parent)
        self.canvas = Canvas(self, width=self.SIZE, height=self.SIZE, background='black')
        self.canvas.pack()
        self.width = self.height = self.SIZE
        self.parent = parent
        self.image = PhotoImage(width=self.width, height=self.height)
        self.parent.client.onEngine = self.onEngine
        self.parent.client.onVictory = self.onVictory
        self.pack()

    def onEngine(self, engine):
        self.run_on_UI_thread(lambda: self.__update_map(engine[2], engine[1]))

    def __update_map(self, players_stats, arr):
        for i in len(arr):
            for j in len(arr[i]):
                if arr[i][j] == -2:
                    self.image.put("#FFFFFF", (i,j))
                elif arr[i][j] == -1:
                    self.image.put("#000000", (i,j))
                else:
                    self.image.put(self.parent.client.players[arr[i][j]][1], (i,j))
        self.canvas.create_image((self.width/2, self.height/2), image=self.image, state=normal)

    def onVictory(self, message):
        self.run_on_UI_thread(lambda: showinfo("Game Over", message))
