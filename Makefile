SRCS = src/configReader.cpp src/main.cpp src/connectionManager.cpp src/messageHandler.cpp

all:
	g++ -std=c++17 $(SRCS) -pthread -o peerProcess

clean:
	rm -f peerProcess