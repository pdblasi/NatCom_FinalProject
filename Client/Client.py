import thread
import Queue
from socket import *
from select import select
import time
from ctypes import *

class Client:
    PORT = 35015
    BUFFER = 1024

    def __init__(self, ip, onClose):
        self.onClose = onClose
        self.HOST = ip
        self.socket = socket()
        self.socket.connect((self.HOST, self.PORT))
        self.socket.setblocking(0)
        self.ready = False
        self.messages = Queue.Queue(10)

    def lobby(self, onMessage, onStart):
        self.onMessage = onMessage
        thread.start_new_thread(self._lobbyHandler, ())

    def close(self):
        self.socket.close()
        self.onClose()

    def set_ready(self, ready):
        self.ready = ready
        self.messages.put([bytearray(c_bool(self.ready))])

    def _lobbyHandler(self):
        while 1:
            try:
                data = self.socket.recv(self.BUFFER)
                if len(data) > 0:
                    self.onMessage(data)
            except:
                pass
            try:
                if not self.messages.empty():
                    message = self.messages.get()

                    self.socket.send(message[0])
                    if len(message) == 2:
                        self.socket.send(message[1])
            except:
                pass
            time.sleep(0.1)
        

if __name__ == '__main__':
    def onMessage(data):
        print '\t', data

    def onClose():
        print "Socket closed."
        return

    def onStart():
        return

    client = Client("192.168.1.105", onClose)
    client.lobby(onMessage, onStart)

    while str(raw_input("Change ready? y/n: ")) == "y":
        client.set_ready(not client.ready)

    client.close()