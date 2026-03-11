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

  MessageType Message::getType() {
      return type;
  }

  std::vector<uint8_t> Message::getPayload() {
      return payload;
  }

  std::vector<uint8_t> Message::serialize() {
      // Format: [4-byte length][1-byte type][payload]
      // Length = size of (type + payload), not including length field itself

      uint32_t length = 1 + payload.size();  // 1 byte for type + payload size
      uint32_t netLength = htonl(length);

      std::vector<uint8_t> result;
      result.resize(4 + 1 + payload.size());  // length field + type + payload

      memcpy(result.data(), &netLength, 4);           // bytes 0-3 inclusive: length
      result[4] = static_cast<uint8_t>(type);         // byte 4 singleton: type

      if (!payload.empty()) {
          memcpy(result.data() + 5, payload.data(), payload.size());  // (exclusive of 4 inclusive of 5) bytes 5+ : payload
      }

      return result;
  }