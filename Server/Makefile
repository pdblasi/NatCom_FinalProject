OBJS = server.o ServerLobby.o ServerEngine.o ServerComm.o

server: server.o ServerLobby.o ServerEngine.o ServerComm.o
	g++ -g -lm ${OBJS} -o server -std=c++11

server.o: server.cpp
	g++ -c -g -lm server.cpp -std=c++11
ServerLobby.o: ServerLobby.cpp ServerLobby.h ServerComm.o
	g++ -c -g -lm ServerLobby.cpp -std=c++11
ServerEngine.o: ServerEngine.cpp player.h ServerEngine.h
	g++ -c -g -lm ServerEngine.cpp -std=c++11
ServerComm.o: ServerComm.cpp ServerComm.h
	g++ -c -g -lm ServerComm.cpp -std=c++11

clean:
	rm *.o server
