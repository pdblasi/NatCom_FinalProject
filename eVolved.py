from Tkinter import *
from App import App

def main():
    root = Tk()
    root.geometry("500x500")
    root.resizable(width=False, height=False)
    app = App.App(root)

    root.mainloop()

if __name__ == '__main__':
    main() 