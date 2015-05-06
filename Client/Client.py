import thread
import Queue
import time
import struct
from Population import *
from socket import *
from ctypes import *
from select import select

class Client:
    PORT = 35015
    HEADER = 4
    TYPE = 1

    READY = 0
    PLAYER_UPDATES = 1
    INDEX = 2
    ENGINE = 3
    VICTORY = 4

    def __init__(self, ip):
        self.HOST = ip
        self.socket = socket()
        self.socket.connect((self.HOST, self.PORT))
        self.socket.setblocking(0)
        self.ready = False
        self.finished = False
        self.messages = Queue.Queue(10)
        self.population = EA(5000)

    def start(self, onReady, onPlayerUpdate, onEngine, onVictory):
        self.onReady = onReady
        self.onPlayerUpdate = onPlayerUpdate
        self.onEngine = onEngine
        self.onVictory = onVictory
        thread.start_new_thread(self._messageHandler, ())

    def close(self):
        self.socket.close()

    def toggle_ready(self):
        self.ready = not self.ready
        self.messages.put([0, 1, bytearray(c_bool(self.ready))])

    def change_color(self, color):
        self.messages.put([1, len(color), color])

    def _messageHandler(self):
        while not self.finished:
            try:
                data = self.socket.recv(self.TYPE)
                if len(data) > 0:
                    type, = struct.unpack('b', data)
                    message = self.socket.recv(struct.unpack('i', self.socket.recv(self.HEADER))[0])
                    if type == self.READY:
                        countdown = int(message)
                        if self.onReady != None:
                            self.onReady(countdown)
                        if countdown == 0:
                            thread.start_new_thread(self._startEngine, ())
                    elif type == self.PLAYER_UPDATES:
                        updates = eval(message)
                        self.player_index = updates[0]
                        self.players = updates[1]
                        if self.onPlayerUpdate != None:
                            self.onPlayerUpdate(updates)
                    elif type == self.ENGINE:
                        engine = eval(engine)
                        if self.onEngine != None:
                            self.onEngine(engine)
                    elif type == self.VICTORY:
                        self.finished = true
                        winner = int(message)
                        if self.onVictory != None:
                            if winner == self.player_index:
                                self.onVictory("You won!")
                            else:
                                self.onVictory("Player {0} won!".format(message + 1))
            except:
                pass
            try:
                if not self.messages.empty():
                    message = self.messages.get()

                    self.socket.send(bytearray(c_byte(message[0])))
                    self.socket.send(bytearray(c_int(message[1])))
                    self.socket.send(message[2])
            except:
                pass
            time.sleep(0.1)

    def _startEngine(self):
        while not self.finished:
            d_pop = len(self.population.population)
            self.population.next_generation()
            d_pop = len(self.population.population) - d_pop
            stats = self.population.stats[:]
            stats.insert(0, d_pop)
            message = "{0} {1} {2} {3} {4}".format(stats[0], stats[1], stats[2], stats[3], stats[4], stats[5])
            self.messages.put([3, len(message), message])


        

if __name__ == '__main__':
    def onReady(countdown):
        print '\t', countdown

    def onPlayerUpdate(updates):
        print '\tMy Index:', updates[0]
        print '\tPlayers:', updates[1]

    def onEngine(engine):
        print 'Population size:', engine[0]
        print 'Population:'
        print engine[1]

    def onVictory(victoryMessage):
        print victoryMessage

    client = Client("192.168.1.105")
    client.start(onReady, onPlayerUpdate, onEngine, onVictory)

    while str(raw_input("Change ready? y/n: ")) == "y":
        client.toggle_ready()

    client.close()
