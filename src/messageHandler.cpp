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

  // Constructor for messages with raw data (bitfield, piece)
  Message::Message(MessageType t, std::vector<uint8_t> data) : type(t), payload(data) {
      // payload is copied directly
  }