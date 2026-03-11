#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <vector>

/*
We want this Message class to handle the byte-level details so int connectionManager::exchange(int fd, int id) only needs to do the handshake logic.

After handshake, it should enter a loop to:
    1. receive messages (bitfield, interested, request, piece, etc.)
    2. send messages back

    This is where we would use the Message class
    1. Create message: Message msg(HAVE, pieceIndex);
    2. Convert to bytes for sending: msg.serialize();
    3. Parse received bytes: Message::deserialize(buffer, len);

    This .h file would be used by connectionManager::exchange() after the handshake to send/receive protocol messages .

    Without this class:
    - every send/receive would require a manual memcpy and byte manipulation
    - would create more surface area to get byte order incorrectly or get the offsets wrong.
    */

enum MessageType {
    CHOKE = 0,
    UNCHOKE = 1,
    INTERESTED = 2,
    NOT_INTERESTED = 3,
    HAVE = 4,
    BITFIELD = 5,
    REQUEST = 6,
    PIECE = 7
};

class Message {
private:
    MessageType type;
    std::vector<uint8_t> payload;

public:
    Message(MessageType t);
    Message(MessageType t, uint32_t pieceIndex);  // for have, request
    Message(MessageType t, std::vector<uint8_t> data);  // for bitfield, piece

    MessageType getType();
    std::vector<uint8_t> getPayload();

    std::vector<uint8_t> serialize();  // convert to bytes for sending
    static Message deserialize(const uint8_t* data, uint32_t length);  // parse received bytes
};

#endif