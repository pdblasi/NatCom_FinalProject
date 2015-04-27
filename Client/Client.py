import thread
from socket import *

class Client:
    PORT = 35015
    BUFFER = 1024

    def __init__(self, ip, onClose):
        self.onClose = onClose
        self.HOST = ip
        self.socket = socket()
        self.socket.connect((self.HOST, self.PORT))

    def lobby(self, onMessage, onStart):
        self.onMessage = onMessage
        thread.start_new_thread(self._lobbyHandler, ())

    def close(self):
        self.socket.close()
        self.onClose()

    def ready(ready):
        return

    def _lobbyHandler(self):
        data = self.socket.recv(self.BUFFER)
        self.onMessage(data)
        

if __name__ == '__main__':
    def onMessage(data):
        print data
        success = input("Quit? y/n: ")
        if success == "y":
            client.close()

    def onClose():
        print "Socket closed."
        return

    def onStart():
        return

    client = Client("192.168.1.114", onClose)
    client.lobby(onMessage, onStart)
