from Tkinter import *
import os

class Example(Frame):
  
    def __init__(self, parent):
        Frame.__init__(self, parent, background="white")   
         
        self.parent = parent
        
        self.initUI()
    
    def initUI(self):

        filename = os.path.abspath(os.path.join(os.path.dirname(__file__), 'Resources'))
        filename = os.path.join(filename, 'tree_of_life.gif')
        image = PhotoImage(file=filename)
        #background = ImageTk.PhotoImage(image)
        background_label = Label(self.parent, image=image)
        self.image = image
        background_label.place(anchor='c', relx=.5, rely=.5)
      
        self.parent.title("eVolved")
        self.pack(fill=BOTH, expand=1)
        

def main():
  
    root = Tk()
    root.geometry("750x500+300+300")
    app = Example(root)
    root.mainloop()  


if __name__ == '__main__':
    main() 