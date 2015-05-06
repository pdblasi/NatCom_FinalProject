from Tkinter import *
from tkMessageBox import *

class Game(Canvas):
    SIZE = 500

    def __init__(self, parent):
        Canvas.__init__(self, parent, width=self.SIZE, height=self.SIZE)
        self.width = self.height = self.SIZE
        self.parent = parent
        self.image = PhotoImage(width=self.width, height=self.height)
        self.create_image((self.width/2, self.height/2), image=self.image, state="normal")
        self.parent.client.onEngine = self.onEngine
        self.parent.client.onVictory = self.onVictory

    def onEngine(self, engine):
        players_stats = engine[1]
        map = engine[2]
        for i in len(map):
            for j in len(map[i]):
                if map[i][j] == -2:
                    self.image.put("#FFFFFF", (i,j))
                elif map[i][j] == -1:
                    self.image.put("#000000", (i,j))
                else:
                    self.image.put(self.parent.client.players[map[i][j]][1], (i,j))

    def onVictory(self, message):
        showinfo("Game Over", message)
