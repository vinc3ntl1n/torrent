#include "connectionManager.h"
#include <cstdlib>    // rand
#include <algorithm>  // sort

bool connectionManager::recvAll(int fd, uint8_t* buf, int len) {
    int total = 0;
    while (total < len) {
        int n = recv(fd, (char*)(buf + total), len - total, 0);
        if (n <= 0) return false;
        total += n;
    }
    return true;
}

bool connectionManager::sendAll(int fd, const uint8_t* buf, int len) {
    int total = 0;
    while (total < len) {
        int n = send(fd, (const char*)(buf + total), len - total, 0);
        if (n <= 0) return false;
        total += n;
    }
    return true;
}

void connectionManager::sendMessage(int fd, Message& msg) {
    std::vector<uint8_t> data = msg.serialize();
    sendAll(fd, data.data(), data.size());
}

void connectionManager::broadcastHave(int pieceIndex) {
    Message haveMsg(HAVE, (uint32_t)pieceIndex);
    std::vector<uint8_t> data = haveMsg.serialize();
    std::lock_guard<std::mutex> lock(fdMutex);
    for (auto& [peerID, peerFd] : peerFDs) {
        sendAll(peerFd, data.data(), data.size());
    }
}

connectionManager::connectionManager(PeerState* ps, FileManager* fm, Logger* lg, int numPeers, int peerID) {
    this->peerState = ps;
    this->fileManager = fm;
    this->logger = lg;
    this->totalPeers = numPeers;
    this->myID = peerID;
}


int connectionManager::startServer(int port, int id) {
    int connection_fd, opt = 1;
    struct sockaddr_in address;
    struct sockaddr_storage;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    if((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        std::cout << "socket error" << std::endl;
        return -1;
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cout << "setsockopt error" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "failed to bind" << std::endl;
        return -1;
    }

    if(listen(server_fd, 4) < 0) {
        std::cout << "listening error" << std::endl;
        return -1;
    }

    while(!done) {
        if((connection_fd = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0) {
            break;
        }

        allServerThreads.push_back(connection_fd);
        std::thread serverWorker(&connectionManager::exchange, this, connection_fd, id, true);
        serverWorker.detach();

    }

    close(server_fd);

    return 0;
}

int connectionManager::connectToPeer(int port, const char* address, int id) {
    struct addrinfo hints, *servinfo, *p;
    int val, client_fd;
    char s[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((val = getaddrinfo(address, std::to_string(port).c_str(), &hints, &servinfo)) != 0) {
        std::cout << "error getting addrinfo" << std::endl;
        return -1;
    }



    for(p = servinfo; p != NULL; p = p->ai_next){
        if((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cout << "next client socket" << std::endl;
            continue;
        }

        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(p->ai_family, &(ipv4->sin_addr), s, sizeof(s));

        if(connect(client_fd, p->ai_addr, p->ai_addrlen) == -1) {
            std::cout << "error connection to server" << std::endl;
            close(client_fd);
            continue;
        }

        break;
    }

    if(p == nullptr) {
        std::cout << "could not bind a socket" << std::endl;
        return -1;
    }

    freeaddrinfo(servinfo);

    allClientThreads.push_back(client_fd);
    std::thread clientWorker(&connectionManager::exchange, this, client_fd, id, false);
    clientWorker.detach();

    return 0;
}

int connectionManager::exchange(int fd, int id, bool isServer) {
    char handshake[32], buffer[33] = { 0 };
    int numberOfBytes;

    memset(handshake, 0, sizeof(handshake));
    memcpy(handshake, "P2PFILESHARINGPROJ", 18);
    uint32_t netId = htonl(id);
    memcpy(handshake + 28, &netId, 4);

    if(send(fd, handshake, sizeof(handshake), 0) == -1) {
        std::cout << "error sending handshake" << std::endl;
        return -1;
    }

    if((numberOfBytes = recv(fd, buffer, 32, 0)) == -1) {
        std::cout << "recv error" << std::endl;
        return -1;
    }

    if(memcmp(buffer, "P2PFILESHARINGPROJ", 18) != 0) {
        std::cout << "invalid handshake header" << std::endl;
        return -1;
    }

    uint32_t receivedId;
    memcpy(&receivedId, buffer + 28, 4);
    receivedId = ntohl(receivedId);

    int theirID = (int)receivedId;
    this->peerState->addNeighbor(theirID);

    if (isServer) {
        std::cout << "[" << id << "] Connected from peer " << theirID << std::endl;
        logger->log("Peer " + std::to_string(id) + " is connected from Peer " + std::to_string(theirID) + ".");
    } else {
        std::cout << "[" << id << "] Connected to peer " << theirID << std::endl;
        logger->log("Peer " + std::to_string(id) + " makes a connection to Peer " + std::to_string(theirID) + ".");
    }

    {
        std::lock_guard<std::mutex> lock(fdMutex);
        peerFDs[theirID] = fd;
    }

    std::vector<uint8_t> myBitfield = peerState->getMyBitfield();
    Message bitfieldMsg(BITFIELD, myBitfield);
    sendMessage(fd, bitfieldMsg);

    uint8_t lenBuf[4];
    if (!recvAll(fd, lenBuf, 4)) return -1;
    uint32_t msgLen;
    memcpy(&msgLen, lenBuf, 4);
    msgLen = ntohl(msgLen);

    std::vector<uint8_t> msgBuf(msgLen);
    if (!recvAll(fd, msgBuf.data(), msgLen)) return -1;

    Message firstMsg = Message::deserialize(msgBuf.data(), msgLen);

    bool gotBitfield = false;
    if (firstMsg.getType() == BITFIELD) {
        peerState->setNeighborBitfield(theirID, firstMsg.getPayload());
        gotBitfield = true;
    }

    bool dominated = false;
    if (gotBitfield) {
        std::vector<uint8_t> theirBits = firstMsg.getPayload();
        std::vector<uint8_t> myBits = peerState->getMyBitfield();
        for (int i = 0; i < (int)myBits.size(); i++) {
            if (theirBits[i] & ~myBits[i]) {
                dominated = true;
                break;
            }
        }
    }

    if (dominated) {
        Message intMsg(INTERESTED);
        sendMessage(fd, intMsg);
    } else {
        Message notIntMsg(NOT_INTERESTED);
        sendMessage(fd, notIntMsg);
    }

    // if first message wasn't bitfield, handle it now as a regular message
    if (!gotBitfield) {
        // re-enter the switch below by processing firstMsg before the loop
        std::vector<uint8_t> payload = firstMsg.getPayload();
        switch (firstMsg.getType()) {
            case INTERESTED:
                peerState->setTheyAreInterested(theirID, true);
                logger->log("Peer " + std::to_string(id) + " received the 'interested' message from " + std::to_string(theirID) + ".");
                break;
            case NOT_INTERESTED:
                peerState->setTheyAreInterested(theirID, false);
                logger->log("Peer " + std::to_string(id) + " received the 'not interested' message from " + std::to_string(theirID) + ".");
                break;
            default: break;
        }
    }

    while (true) {
        if (!recvAll(fd, lenBuf, 4)) break;
        memcpy(&msgLen, lenBuf, 4);
        msgLen = ntohl(msgLen);

        if (msgLen == 0) continue;

        std::vector<uint8_t> body(msgLen);
        if (!recvAll(fd, body.data(), msgLen)) break;

        Message msg = Message::deserialize(body.data(), msgLen);
        std::vector<uint8_t> payload = msg.getPayload();

        switch (msg.getType()) {
            case CHOKE: {
                peerState->setIAmChoked(theirID, true);
                logger->log("Peer " + std::to_string(id) + " is choked by " + std::to_string(theirID) + ".");
                {
                    std::lock_guard<std::mutex> rlock(requestMutex);
                    if (pendingPerPeer.count(theirID)) {
                        requestedPieces.erase(pendingPerPeer[theirID]);
                        pendingPerPeer.erase(theirID);
                    }
                }
                break;
            }
            case UNCHOKE: {
                peerState->setIAmChoked(theirID, false);
                logger->log("Peer " + std::to_string(id) + " is unchoked by " + std::to_string(theirID) + ".");
                std::vector<int> candidates;
                {
                    std::lock_guard<std::mutex> rlock(requestMutex);
                    for (int i = 0; i < (int)(peerState->getMyBitfield().size() * 8); i++) {
                        if (!peerState->hasPiece(i) && peerState->neighborHasPiece(theirID, i) && requestedPieces.find(i) == requestedPieces.end()) {
                            candidates.push_back(i);
                        }
                    }
                    if (!candidates.empty()) {
                        int pick = candidates[rand() % candidates.size()];
                        requestedPieces.insert(pick);
                        pendingPerPeer[theirID] = pick;
                        Message req(REQUEST, (uint32_t)pick);
                        sendMessage(fd, req);
                    }
                }
                break;
            }
            case INTERESTED: {
                peerState->setTheyAreInterested(theirID, true);
                logger->log("Peer " + std::to_string(id) + " received the 'interested' message from " + std::to_string(theirID) + ".");
                break;
            }
            case NOT_INTERESTED: {
                peerState->setTheyAreInterested(theirID, false);
                logger->log("Peer " + std::to_string(id) + " received the 'not interested' message from " + std::to_string(theirID) + ".");
                break;
            }
            case HAVE: {
                uint32_t pieceIdx;
                memcpy(&pieceIdx, payload.data(), 4);
                pieceIdx = ntohl(pieceIdx);
                peerState->setNeighborPiece(theirID, pieceIdx);
                logger->log("Peer " + std::to_string(id) + " received the 'have' message from " + std::to_string(theirID) + " for the piece " + std::to_string(pieceIdx) + ".");

                if (!peerState->hasPiece(pieceIdx)) {
                    Message intMsg(INTERESTED);
                    sendMessage(fd, intMsg);
                }

                if (peerState->allComplete(totalPeers)) break;
                break;
            }
            case REQUEST: {
                uint32_t pieceIdx;
                memcpy(&pieceIdx, payload.data(), 4);
                pieceIdx = ntohl(pieceIdx);

                std::vector<uint8_t> pieceData = fileManager->readPiece(pieceIdx);
                uint32_t netIdx = htonl(pieceIdx);
                std::vector<uint8_t> piecePayload(4 + pieceData.size());
                memcpy(piecePayload.data(), &netIdx, 4);
                memcpy(piecePayload.data() + 4, pieceData.data(), pieceData.size());

                Message pieceMsg(PIECE, piecePayload);
                sendMessage(fd, pieceMsg);
                break;
            }
            case PIECE: {
                uint32_t pieceIdx;
                memcpy(&pieceIdx, payload.data(), 4);
                pieceIdx = ntohl(pieceIdx);

                std::vector<uint8_t> pieceData(payload.begin() + 4, payload.end());
                fileManager->writePiece(pieceIdx, pieceData);
                peerState->setPiece(pieceIdx);
                peerState->addDownloaded(theirID, pieceData.size());

                {
                    std::lock_guard<std::mutex> rlock(requestMutex);
                    requestedPieces.erase(pieceIdx);
                    pendingPerPeer.erase(theirID);
                }

                int pieceCount = peerState->countMyPieces();
                std::cout << "[" << id << "] Piece " << pieceIdx << " from " << theirID << " (" << pieceCount << "/" << peerState->getTotalPieces() << ")" << std::endl;
                logger->log("Peer " + std::to_string(id) + " has downloaded the piece " + std::to_string(pieceIdx) + " from " + std::to_string(theirID) + ". Now the number of pieces it has is " + std::to_string(pieceCount) + ".");

                if (peerState->isComplete()) {
                    std::cout << "[" << id << "] Download complete!" << std::endl;
                    logger->log("Peer " + std::to_string(id) + " has downloaded the complete file.");
                }

                broadcastHave(pieceIdx);

                // after getting a piece, check if we should send NOT_INTERESTED to any neighbor
                {
                    std::lock_guard<std::mutex> lock(fdMutex);
                    for (auto& [pid, pfd] : peerFDs) {
                        bool theyHaveSomethingWeNeed = false;
                        for (int i = 0; i < (int)(peerState->getMyBitfield().size() * 8); i++) {
                            if (!peerState->hasPiece(i) && peerState->neighborHasPiece(pid, i)) {
                                theyHaveSomethingWeNeed = true;
                                break;
                            }
                        }
                        if (!theyHaveSomethingWeNeed) {
                            Message notInt(NOT_INTERESTED);
                            sendMessage(pfd, notInt);
                        }
                    }
                }

                if (peerState->allComplete(totalPeers)) break;

                if (!peerState->amIChokedBy(theirID)) {
                    std::vector<int> candidates;
                    std::lock_guard<std::mutex> rlock(requestMutex);
                    for (int i = 0; i < (int)(peerState->getMyBitfield().size() * 8); i++) {
                        if (!peerState->hasPiece(i) && peerState->neighborHasPiece(theirID, i) && requestedPieces.find(i) == requestedPieces.end()) {
                            candidates.push_back(i);
                        }
                    }
                    if (!candidates.empty()) {
                        int pick = candidates[rand() % candidates.size()];
                        requestedPieces.insert(pick);
                        pendingPerPeer[theirID] = pick;
                        Message req(REQUEST, (uint32_t)pick);
                        sendMessage(fd, req);
                    }
                }
                break;
            }
        }

        if (peerState->allComplete(totalPeers)) {
            exit(0);
        }
    }

    {
        std::lock_guard<std::mutex> lock(fdMutex);
        peerFDs.erase(theirID);
    }

    return 0;
}

void connectionManager::closePeers(std::vector<int> threads) {
    for(int thread : threads) {
        close(thread);
    }
}

void connectionManager::closeConnections() {
    closePeers(allServerThreads);
    closePeers(allClientThreads);
}

void connectionManager::unchokingTimer(int interval, int k) {
    while (true) {
        sleep(interval);

        peerState->lock();

        std::vector<int> interested = peerState->getInterestedNeighbors();

        if (!peerState->isComplete()) {
            std::sort(interested.begin(), interested.end(), [&](int a, int b) {
                return peerState->getDownloadRate(a) > peerState->getDownloadRate(b);
            });
        } else {
            for (int i = interested.size() - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                std::swap(interested[i], interested[j]);
            }
        }

        std::string preferredList;
        std::lock_guard<std::mutex> lock(fdMutex);
        for (int i = 0; i < (int)interested.size(); i++) {
            int pid = interested[i];
            if (i < k) {
                if (!preferredList.empty()) preferredList += ", ";
                preferredList += std::to_string(pid);
                if (peerState->amIChoking(pid)) {
                    peerState->setChoking(pid, false);
                    if (peerFDs.count(pid)) {
                        Message msg(UNCHOKE);
                        sendMessage(peerFDs[pid], msg);
                    }
                }
            } else if (pid != optimisticPeer) {
                if (!peerState->amIChoking(pid)) {
                    peerState->setChoking(pid, true);
                    if (peerFDs.count(pid)) {
                        Message msg(CHOKE);
                        sendMessage(peerFDs[pid], msg);
                    }
                }
            }
        }

        if (!preferredList.empty()) {
            logger->log("Peer " + std::to_string(myID) + " has the preferred neighbors " + preferredList + ".");
        }

        peerState->resetAllDownloadRates();
        peerState->unlock();
    }
}

void connectionManager::optimisticTimer(int interval) {
    while (true) {
        sleep(interval);

        peerState->lock();

        std::vector<int> candidates = peerState->getChokedInterestedNeighbors();

        if (!candidates.empty()) {
            int pick = candidates[rand() % candidates.size()];
            optimisticPeer = pick;
            peerState->setChoking(pick, false);
            logger->log("Peer " + std::to_string(myID) + " has the optimistically unchoked neighbor " + std::to_string(pick) + ".");

            std::lock_guard<std::mutex> lock(fdMutex);
            if (peerFDs.count(pick)) {
                Message msg(UNCHOKE);
                sendMessage(peerFDs[pick], msg);
            }
        }

        peerState->unlock();
    }
}