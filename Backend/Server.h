#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include<condition_variable>
#include "Message.h"
#include "UDPSocket.h"
#include "ParseUtil.h"


using namespace std;
struct Cache
{
  int rem;
  vector<char *> msgs;
  Cache() {}
  Cache(int len) : rem(len)
  {
    msgs.assign(rem, nullptr);
  }
};
class Server
{
protected:
  UDPSocket *udpSocket;
  map<int, condition_variable*> rpcToMutex;
  map<int, Message*> rpcToMsg;

  Message *getRequest(unsigned int &, int &);
  virtual Message *doOperation(Message *_received, unsigned int, unsigned int);
  void sendReply(Message *_message);
  bool idempotent(int operation_id);
  bool running;
  map<unsigned int, map<int, map<int, Message *>>> history;
  map<unsigned int, map<int, map<int, Cache>>> partsCache;
  void serveRequestNewThread(Message *, unsigned int, unsigned int);
  Message *processPart(Message *, unsigned int, int);
  void sendPartitioned(Message *_message);
  mutex historyMutex;
  



public:
  Server(char *_listen_hostname, int _listen_port);
  void serveRequest();
  bool isRunning();
  ~Server();
};
#endif // SERVER_H
