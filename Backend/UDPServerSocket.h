#ifndef UDPSERVERSOCKET_H
#define UDPSERVERSOCKET_H
#include "UDPSocket.h"
class UDPServerSocket : public UDPSocket
{
    public:
        UDPServerSocket ();
        ~UDPServerSocket ( );
};
#include "UDPServerSocket.cpp"
#endif // UDPSERVERSOCKET_H
