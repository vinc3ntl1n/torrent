SRCS = src/configReader.cpp src/main.cpp src/connectionManager.cpp src/messageHandler.cpp src/peerState.cpp src/fileManager.cpp src/logger.cpp

all:
	g++ -std=c++17 $(SRCS) -pthread -o peerProcess

setup-small:
	cp config/unzipped/project_config_file_small/project_config_file_small/Common.cfg Common.cfg
	cp config/unzipped/project_config_file_small/project_config_file_small/PeerInfo.cfg PeerInfo.cfg
	mkdir -p peer_1001 peer_1002 peer_1003 peer_1004 peer_1005 peer_1006 peer_1007 peer_1008 peer_1009
	cp config/unzipped/project_config_file_small/project_config_file_small/1001/thefile peer_1001/thefile
	cp config/unzipped/project_config_file_small/project_config_file_small/1006/thefile peer_1006/thefile

setup-large:
	cp config/unzipped/project_config_file_large/project_config_file_large/Common.cfg Common.cfg
	cp config/unzipped/project_config_file_large/project_config_file_large/PeerInfo.cfg PeerInfo.cfg
	mkdir -p peer_1001 peer_1002 peer_1003 peer_1004 peer_1005 peer_1006
	cp config/unzipped/project_config_file_large/project_config_file_large/1001/tree.jpg peer_1001/tree.jpg

clean:
	rm -f peerProcess Common.cfg PeerInfo.cfg
	rm -rf peer_1001 peer_1002 peer_1003 peer_1004 peer_1005 peer_1006 peer_1007 peer_1008 peer_1009
	rm -f log_peer_*.log
