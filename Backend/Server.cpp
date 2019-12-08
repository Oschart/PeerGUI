#include "Server.h"
#define MAX_SIZE 8192
#define MAX_PART_SIZE 6130

int lastPacketSize;

Message *Server::getRequest(unsigned int &peerIP, int& peerPort)
{
  char *messageBuf = new char[MAX_SIZE];
  int receivedL = this->udpSocket->readSocketWithBlock(messageBuf, MAX_SIZE);

  cout << "Got Request\n";
  peerIP = this->udpSocket->getPeerAddressCoded();
  peerPort = this->udpSocket->getPeerPort();

  if (receivedL < 0)
  {
    cout << "Error: No message received\n";
    delete[] messageBuf;
    return nullptr;
  }
  Message *received = new Message(messageBuf);
  delete[] messageBuf;
  if (received->getMessageType() == Request)
  {
    //cout << "Message received: "; cout << "$" << (char*)received->getMessage() <<"$" <<  endl;
    return received;
  }
  else
  {
    if (received != nullptr && received->getTotal() > 1){
        received = processPart(received, peerIP, peerPort);
      }

      if (received != nullptr ) {
        cout << "Reply received\n" << endl;
        char* cont = (char*)received->getMessage();
        cout << "Message content: " << cont << endl;
        int rpc_id = received->getRPCId();
            cout << "This bitch should wake up : " << rpc_id << endl;
        if (this->rpcToMutex.count(rpc_id)){
            cout << "IS THERE " << rpc_id << endl;
            rpcToMsg[rpc_id] = received;
            rpcToMutex[rpc_id]->notify_one();
            rpcToMutex.erase(rpc_id);
        }
      }
    return nullptr;
  }
}

// Note: this method recycles the received message, it doesn't deep copy it
Message *Server::doOperation(Message *original, unsigned int peerIP, unsigned int peerPort)
{
  Message *response = original;
  void *vPtr = response->getMessage();

  char *mesg = static_cast<char *>(vPtr);
  cout << mesg << endl;

  if (!strcmp(mesg, "q"))
  {
    this->running = 0;
    cout << "The server will terminate\n";
    char *terminating_reply = "Server Terminated!";
    response->setMessage(terminating_reply, strlen(terminating_reply));
  }
  return response;
}

void Server::sendReply(Message *_message)
{
  _message->setMessageType(Reply);
  sendPartitioned(_message);
  // char *messageBuf = new char[MAX_SIZE];
  // messageBuf = _message->marshal();
  // cout << "Will send: " << (char*)_message->getMessage()<< endl;
  // int result = this->udpSocket->writeToSocket(messageBuf, MAX_SIZE);
  // if (result < 0)
  //   cout << "Error: Couldn't send message\n";
  //
  // delete[] messageBuf;
}

Server::Server(char *_listen_hostname, int _listen_port)
{
  this->udpSocket = new UDPSocket();
  this->udpSocket->initializeServer(_listen_hostname, _listen_port);
  this->running = 1;
}

Message* Server::processPart (Message* incoming, unsigned int peerIP, int peerPort){

    //cout << "Process Part = " << incoming->getMessageSize() << endl;
  int partIndex = incoming->getIndex();
  int message_rpc_id = incoming->getRPCId();

  if (partsCache.count(peerIP) &&
      partsCache[peerIP].count(peerPort) &&
      partsCache[peerIP][peerPort].count(message_rpc_id)){
      auto & cache = partsCache[peerIP][peerPort][message_rpc_id];
      if (cache.rem == -1) return nullptr;
    if (cache.msgs[partIndex] == nullptr){
      if (partIndex == cache.msgs.size()-1) {
          lastPacketSize = incoming->getMessageSize();
          cout << "final packet" << endl;
          char * c = (char*)incoming->getMessage();
          cout << int(c[lastPacketSize-1]) << endl;
      }
      char * msg = (char*) incoming->getMessage();
      cache.msgs[partIndex] = new char[incoming->getMessageSize()];
      memcpy(cache.msgs[partIndex], msg, incoming->getMessageSize());
      --cache.rem;
      cout << "remaining " << cache.rem << endl;
      if (cache.rem == 0){
        string recollected;
        for (int i = 0 ; i < cache.msgs.size(); i++){
          recollected += string(cache.msgs[i], (i == cache.msgs.size()-1 ? lastPacketSize : MAX_PART_SIZE));
          delete [] cache.msgs[i];
        }

        cout << "Recollected random = " << (int)recollected.back() << endl;
        cout << "The size of recollected " << recollected.length() << endl;
        incoming->setMessage(recollected.c_str(), recollected.length());
        partsCache[peerIP][peerPort][message_rpc_id].rem = -1;
      }
      else {
        delete incoming;
        incoming = nullptr;
      }
    }
    else {
      delete incoming;
      incoming = nullptr;
    }
  }
  else {
    partsCache[peerIP][peerPort][message_rpc_id] = Cache(incoming->getTotal());
    auto & cache = partsCache[peerIP][peerPort][message_rpc_id];
    char * msg = (char*) incoming->getMessage();
    cache.msgs[partIndex] = new char[incoming->getMessageSize()];
    strcpy(cache.msgs[partIndex], msg);
    --cache.rem;
    cout << "remaining " << cache.rem << endl;

    delete incoming;
    incoming = nullptr;
  }
  return incoming;
}

void Server::sendPartitioned(Message *_message)
{
    char *messageBuf = (char *)_message->getMessage();
    int len = _message->getMessageSize();
    cout << "The length to be sent " << len << endl;
    //_message->setMessageType(Request);
    Message part(_message);
    int numOfParts = 1 + (len - 1) / MAX_PART_SIZE;
    //cout << "Will send " << numOfParts << " parts " << endl;
    for (int i = 0; i < len; i += MAX_PART_SIZE)
    { //maximum string without metadata
        int subLen = min(MAX_PART_SIZE, len - i);
        char *subString = new char[subLen];
        memcpy(subString, messageBuf + i, subLen);
        part.setMessage(subString, subLen);
        part.setIndex(i / MAX_PART_SIZE);
        part.setTotal(numOfParts);
        if (i / MAX_PART_SIZE == numOfParts -1) {
            char * c = (char*) part.getMessage();
            //cout << "last char I send " << endl;
            //cout << int(c[subLen-1]) << endl;
        }
        char *marshalled = part.marshal();
        cout << "I will write" << endl;
        this->udpSocket->writeToSocketAndWait(marshalled, MAX_SIZE, 50);
        cout << "I wrote and waited" << endl;


        delete[] marshalled;
        delete[] subString;
    }
}

void Server::serveRequest()
{
  unsigned int peerIP; int peerPort;
  Message *incoming = this->getRequest(peerIP, peerPort);
  //if (incoming != nullptr) cout << "The incoming has " << incoming->getTotal() << " parts " << endl;
  if (incoming != nullptr && incoming->getTotal() > 1){
    incoming = processPart(incoming, peerIP, peerPort);
  }
  if (incoming != nullptr)
  {
    cout << "Collected a message whose size = " << incoming->getMessageSize() << endl;
    thread newService(&Server::serveRequestNewThread, this, incoming, peerIP, peerPort);
    newService.detach();
  }
}

void Server::serveRequestNewThread(Message *incoming, unsigned int peerIP, unsigned int peerPort)
{
  bool doOperation = 0;
  int message_rpc_id = incoming->getRPCId();

  if (!idempotent(incoming->getOperation()))
  {
    historyMutex.lock();
    if (history.count(peerIP) &&
        history[peerIP].count(peerPort) &&
        history[peerIP][peerPort].count(message_rpc_id)){
      if(history[peerIP][peerPort][message_rpc_id] == nullptr){
      //cout << "IN PROGRESS!" << endl;
    } else {
        cout << "DUPLICATE!" << endl;
        Message *outcoming = history[peerIP][peerPort][message_rpc_id];
        sendReply(outcoming);
      }
    }
    else{
      history[peerIP][peerPort][message_rpc_id] = nullptr;
      doOperation = 1;
    }
    historyMutex.unlock();
  }
  else
    doOperation = 1;

  if (doOperation)
  {
    Message *outcoming = this->doOperation(incoming, peerIP, peerPort);
    if (outcoming != nullptr)
    {
      sendReply(outcoming);
      history[peerIP][peerPort][message_rpc_id] = new Message(outcoming);
    }
  }

  if (incoming != nullptr)
    delete incoming;
}

bool Server::idempotent(int operation_id) { return false; }

bool Server::isRunning() { return this->running; }

Server::~Server()
{
  delete this->udpSocket;
}
