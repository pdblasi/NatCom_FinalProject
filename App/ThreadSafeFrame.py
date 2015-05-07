from Tkinter import Frame
from Queue import Queue, Empty

class ThreadSafeFrame(Frame):
    def __init__(self, parent):
        Frame.__init__(self,parent)
        self.__queue = Queue()
        self.after(100, self.__run_queued)

    def __run_queued(self):
        try:
            while True:
                func = self.__queue.get_nowait()
                func()
        except Empty:
            self.after(100, self.__run_queued)

    def run_on_UI_thread(self, func):
        self.__queue.put(func)