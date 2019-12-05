#include "UDPSocket.h"

using namespace std;

UDPSocket::UDPSocket(){
  if(( sock = socket(AF_INET, SOCK_DGRAM, 0))<0) {
    throw runtime_error("Unable to create socket");
  }
  myPort = 0;
  peerPort = -1;
  myAddress = NULL;
  peerAddress = NULL;
  bound = 0;
}

//binds socket to a local socket address
bool UDPSocket::doBind(){
  if( bind(sock, (sockaddr*)&myAddr, sizeof(struct sockaddr_in))!= 0){
    cout << "Unable to bind socket" << endl;
    return 0;
  }
  bound = 1;
  return 1;
}

//initializes the variable myAddr, to be used for binding
void UDPSocket::initializeMyAddr(){
  myAddr.sin_family = AF_INET;
  myAddr.sin_port = htons(myPort);
  if (myAddress == NULL) myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  else inet_pton(AF_INET, myAddress, &myAddr.sin_addr);
}

//designates the socket as a server
bool UDPSocket::initializeServer (char * _myAddr, int _myPort){
  if (myAddress != NULL) free(myAddress);
  myAddress = (char*)malloc(strlen(_myAddr)+1);
  strcpy(myAddress, _myAddr);
  myPort = _myPort;
  initializeMyAddr();
  if (bound){ //if already bound by a client: bind again with updated parameters
    close(sock);
    if(( sock = socket(AF_INET, SOCK_DGRAM, 0))<0) {
      cerr << "Unable to create new socket\n";
      return 0;
    }
  }
  if (!doBind()) return 0;

  return 1;
}

//designates the socket as a client
bool UDPSocket::initializeClient (sockaddr_in _peerAddr){

  peerAddr.sin_family = AF_INET;
  peerAddr.sin_addr = _peerAddr.sin_addr;
  peerAddr.sin_port = _peerAddr.sin_port;
  peerPort = ntohs(_peerAddr.sin_port);
  peerAddrCoded = _peerAddr.sin_addr.s_addr;
}

bool UDPSocket::initializeClient(unsigned int _peerAddrCoded, unsigned int _peerPort){
  peerAddrCoded= _peerAddrCoded;
  peerPort = _peerPort;
  peerAddr.sin_addr.s_addr = peerAddrCoded;
  peerAddr.sin_port = htons(peerPort);
}

unsigned int UDPSocket::getPeerAddressCoded() {
  return peerAddrCoded;
}

//same as above, but uses user-friendly parameters
bool UDPSocket::initializeClient (char * _peerAddr, int _peerPort){
  if (peerAddress != NULL) free(peerAddress);
  peerAddress = (char*)malloc(strlen(_peerAddr)+1);
  strcpy(peerAddress, _peerAddr);
  peerPort = _peerPort;

  if (!bound){ //prevents binding again if already bound by server
    initializeMyAddr();
    if (!doBind()) return 0;
  }

  //begin initializing peerAddr
  struct hostent *host;
  peerAddr.sin_family = AF_INET;
  if((host = gethostbyname(peerAddress))== NULL){
   cerr << "Unknown host name\n";
   return 0;
  }
  peerAddr.sin_addr = *(struct in_addr *) (host->h_addr);
  cout << "the address " << peerAddr.sin_addr.s_addr << endl; 
  peerAddr.sin_port = htons(peerPort);
  //end initializing peerAddr

  return 1;
}

//Writes to the socket without waiting
int UDPSocket::writeToSocket(char * buffer, int maxBytes){
  if (!bound) {cerr << "The socket was not bound" << endl; return -1;}
  if (peerPort == -1) {cerr << "No client has been identified" << endl; return -1;}
  int n;
  char * buff2 = new char[maxBytes]; memcpy(buff2, buffer, maxBytes+1);

  if( (n = sendto(sock, buff2, maxBytes, 0, (sockaddr*)&peerAddr,
    sizeof(struct sockaddr_in))) < 0) {

      	cout << "Error writing to socket:\t";
	cout  << errno << endl;
  }
  delete [] buff2;
  return n;
}

//writes to the socket and waits afterwards
int UDPSocket::writeToSocketAndWait(char * buffer, int maxBytes, int microSec){
  int n = writeToSocket(buffer, maxBytes); //normal writing
  if (n < 0) return n; //if write failed: do not wait
  usleep(microSec); //sleeping
  return n;
}

//reads from socket, blocking until reading
int UDPSocket::readSocketWithBlock (char * buffer, int maxBytes ){
  int n; struct sockaddr_in aSocketAddress; //the sockaddr of the sender
  int aLength = sizeof(aSocketAddress);
  aSocketAddress.sin_family = AF_INET;
  n = recvfrom(sock, buffer, maxBytes, 0, (sockaddr*)&aSocketAddress,
    (socklen_t*)&aLength);

  //if successful, designate the sender as a receiver of our writes
  //This is useful to send replies
  if (n > 0) initializeClient(aSocketAddress);

  return n;
}

//reads from socket, only blocking for a finite period of time
int UDPSocket::readSocketWithTimeout(char * buffer, int maxBytes, int timeoutSec, int timeoutMilli){
  struct timeval tv; //representing the time interval for the timeout
  tv.tv_sec = timeoutSec;
  tv.tv_usec = timeoutMilli*1000;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) { //setting the timeout
      cout << "Error in setting the timeout" << endl;
      return -1;
  }
  int n = readSocketWithBlock(buffer, maxBytes); //normal read
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) { //resetting the timeout
      cout << "Error in disabling the timeout" << endl;
      return -1;
  }
  return n;
}

//reads from socket, without blocking
int UDPSocket::readSocketWithNoBlock(char * buffer, int maxBytes){
  int flags = fcntl(sock, F_GETFL);
  flags |= O_NONBLOCK; //setting the non-blocking bit
  fcntl(sock, F_SETFL, flags);
  int n = readSocketWithBlock(buffer, maxBytes);
  flags = fcntl(sock, F_GETFL);
  flags &= (~O_NONBLOCK); //resetting the non-blocking bit
  fcntl(sock, F_SETFL, flags);
  return n;
}

//returns the socket's port
int UDPSocket::getMyPort() {
  if (bound) return myPort;
  return -1;
}

//returns the (latest) peer's port
int UDPSocket::getPeerPort(){
  return peerPort;
}

void UDPSocket::enable(){
  enabled = true;
}

void UDPSocket::disable(){
  enabled = false;
}

bool UDPSocket::isEnabled() {
  return enabled;
}

//returns the socket handler
int UDPSocket::getSocketHandler() {
  return sock;
}

//closes the socket
UDPSocket::~UDPSocket(){
  close(sock);
}
