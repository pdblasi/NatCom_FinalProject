from Tkinter import *

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