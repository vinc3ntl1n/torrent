#include "messageHandler.h"
#include <arpa/inet.h>  // for htonl, ntohl
#include <cstring>      // for memcpy


/*
Constructor for no-payload messages 
(choke, 
unchoke, 
interested, 
not interested)
*/
Message::Message(MessageType t) : type(t) {
    // payload stays empty
}

// Constructor for messages with piece index (have, request)
  Message::Message(MessageType t, uint32_t pieceIndex) : type(t) {
      uint32_t netIndex = htonl(pieceIndex);  // convert to network byte order
      payload.resize(4);
      memcpy(payload.data(), &netIndex, 4);
  }