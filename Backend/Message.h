#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
using namespace std;

typedef basic_string<unsigned char> ustring;


enum MessageType { Request, Reply};

class Message
{
    private:
        MessageType message_type;
        size_t operation;
        void * message;
        size_t message_size;
        size_t rpc_id;
        size_t totalParts;
        size_t partIndex;
        static const bool verbose = false;
        static void addField(ustring &s, int size, unsigned int value);
        static unsigned int getField(char* pck, int size);
        static ustring base64_encode(char* msg, size_t message_size);
        static ustring base64_decode(char* encoded_string, size_t message_size);
    public:
        Message(int operation, void * p_message, size_t p_message_size, int p_rpc_id);
        Message(char * marshalled_base64);
        Message(Message*);
        char * marshal ();
        int getTotal();
        int getIndex();
        int getOperation ();
        int getRPCId();
        void * getMessage();
        size_t getMessageSize();
        MessageType getMessageType();
        void setTotal(int);
        void setIndex(int);
        void setOperation (int _operation);
        void setMessage (const void * msg, size_t msg_size);
        void setMessageType (MessageType msg_type);
        bool operator==(Message* other);
        ~Message();
};

//#include "Message.cpp"
#endif // MESSAGE_H
