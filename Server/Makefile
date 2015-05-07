OBJS = server.o ServerLobby.o ServerEngine.o ServerComm.o

server: server.o ServerLobby.o ServerEngine.o ServerComm.o
	g++ -g -lm ${OBJS} -o server -std=c++11
server.exe: server.o ServerLobby.o ServerEngine.o ServerComm.o
	g++ -g -lm ${OBJS} -o server.exe -std=c++11 -LC:\Windows\System32 -lws2_32

server.o: server.cpp
	g++ -g -c -lm server.cpp -std=c++11
ServerLobby.o: ServerLobby.cpp ServerLobby.h ServerComm.o
	g++ -g -c -lm ServerLobby.cpp -std=c++11
ServerEngine.o: ServerEngine.cpp player.h ServerEngine.h
	g++ -g -c -lm ServerEngine.cpp -std=c++11
ServerComm.o: ServerComm.cpp ServerComm.h
	g++ -g -c -lm ServerComm.cpp -std=c++11

clean:
	rm *.o server
