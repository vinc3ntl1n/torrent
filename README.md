# Torrent

BitTorrent-like protocol implementation in C++

## Build

```bash
make
```

Or manually:

```bash
g++ -std=c++17 src/configReader.cpp src/main.cpp src/connectionManager.cpp src/messageHandler.cpp src/peerState.cpp src/fileManager.cpp src/logger.cpp -pthread -o peerProcess
```

## Setup

Place `Common.cfg` and `PeerInfo.cfg` in the working directory.

Create a subdirectory `peer_[peerID]/` for each peer. Peers that have the file need the file placed in their subdirectory before starting. Peers that don't have the file will create an empty file automatically.

Example with 6 peers where peer 1001 has the file:

```
./Common.cfg
./PeerInfo.cfg
./peer_1001/thefile # must exist before starting
./peer_1002/        # empty and created before starting
./peer_1003/
./peer_1004/
./peer_1005/
./peer_1006/
```

## Running

Start peers in order listed in `PeerInfo.cfg`:

```bash
./peerProcess 1001    # start first (has file and listens for others)
./peerProcess 1002    # connects to 1001, listens for later peers
./peerProcess 1003    # connects to 1001 and 1002
...
```

Each peer connects to all peers listed before it and listens for peers listed after it. Peers terminate automatically when all peers have the complete file
