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