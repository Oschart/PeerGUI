#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <errno.h>

using namespace std;

class UDPSocket {
private:
  bool bound;
  bool doBind();
  void initializeMyAddr();
protected:
  int sock;
  sockaddr_in myAddr;
  sockaddr_in peerAddr;
  char * myAddress;
  char * peerAddress;
  int myPort;
  int peerPort;
  bool enabled;
  pthread_mutex_t mutex;
  unsigned int peerAddrCoded;
public:
  UDPSocket();
  void setFilterAddress (char * _filterAddress);
  char * getFilterAddress ();
  bool initializeServer (char * _myAddr, int _myPort);
  bool initializeClient (char * _peerAddr, int _peerPort);
  bool initializeClient (sockaddr_in client);
  bool initializeClient (unsigned int, unsigned int);
  int writeToSocket (char * buffer, int maxBytes );
  int writeToSocketAndWait (char * buffer, int maxBytes,int microSec );
  int readFromSocketWithNoBlock (char * buffer, int maxBytes );
  int readFromSocketWithTimeout (char * buffer, int maxBytes, int timeoutSec, int timeoutMilli);
  int readFromSocketWithBlock (char * buffer, int maxBytes );
  int readSocketWithNoBlock (char * buffer, int maxBytes );
  int readSocketWithTimeout (char * buffer, int maxBytes, int timeoutSec, int timeoutMilli);
  int readSocketWithBlock (char * buffer, int maxBytes );
  int getMyPort();
  int getPeerPort ();
  unsigned int getPeerAddressCoded();
  void enable();
  void disable();
  bool isEnabled();
  void lock();
  void unlock();
  int getSocketHandler();
  ~UDPSocket ();
};
#endif // UDPSOCKET_H
