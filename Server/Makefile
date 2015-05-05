ServerLobby.o: ServerLobby.cpp ServerLobby.h ServerComm.o
	g++ -c -lm ServerLobby.cpp -std=c++11
ServerEngine.o: ServerEngine.cpp player.h ServerEngine.h
	g++ -c -lm ServerEngine.cpp -std=c++11
ServerComm.o: ServerComm.cpp ServerComm.h
	g++ -c -lm ServerComm.cpp -std=c++11

clean:
	rm *.o server
