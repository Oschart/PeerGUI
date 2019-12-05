
#include <iostream>
#include "Message.h"
#include <string.h>
#include <stdexcept>

using namespace std;

#define MSG_TYPE_SIZE 1
#define OP_SIZE 1
#define RPC_ID_SIZE 4
#define MSG_SIZE 4
#define TOTAL_PARTS_SIZE 4
#define PART_IDX_SIZE 4

static string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

Message::Message(int operation, void *p_message, size_t p_message_size, int p_rpc_id)
{
  this->message_type = Request;
  this->operation = operation;
  this->message_size = p_message_size;
  this->rpc_id = p_rpc_id;

  this->message = new char[p_message_size + 1];
  memcpy((char *)this->message, (char *)p_message, p_message_size + 1);
}

Message::Message(Message *_other)
{
  this->message_type = _other->message_type;
  this->operation = _other->operation;
  this->message_size = _other->message_size;
  this->rpc_id = _other->rpc_id;

  this->message = new char[message_size + 1];
  memcpy((char *)this->message, (char *)_other->message, message_size + 1);
}

unsigned int Message::getField(char *pck, int size)
{
  unsigned int value = 0;
  for (int i = 0; i < size; ++i)
  {
    value |= ((unsigned char)pck[i] << (8 * i));
  }
  return value;
}

Message::Message(char *marshalled_base64)
{
  char *pck = marshalled_base64;


  this->message_type = (getField(pck, MSG_TYPE_SIZE) == '0' ? Request : Reply);
  pck += MSG_TYPE_SIZE;

  this->rpc_id = getField(pck, RPC_ID_SIZE);
  pck += RPC_ID_SIZE;

  this->operation = getField(pck, OP_SIZE);
  pck += OP_SIZE;

  this->message_size = getField(pck, MSG_SIZE);
  pck += MSG_SIZE;

  this->totalParts = getField(pck, TOTAL_PARTS_SIZE);
  pck += TOTAL_PARTS_SIZE;

  this->partIndex = getField(pck, PART_IDX_SIZE);
  pck += PART_IDX_SIZE;

  marshalled_base64 = pck;


  ustring umars = base64_decode(marshalled_base64, strlen(marshalled_base64));

  try
  {
    char *str = new char[umars.size() + 1];
    memcpy(str, umars.c_str(), umars.size() + 1);
    str[umars.size()] = 0;
    this->message = str;
  }
  catch (exception &e)
  {
    cout << "In Message::Constructor: " << e.what() << endl;
    this->message = NULL;
  }
  //delete[] pck;
  //pck = NULL;
}

void Message::addField(ustring &s, int size, unsigned int value)
{
  for (int i = 0; i < size; ++i)
  {
    s += (unsigned char)((value & (0xff << (8 * i))) >> 8 * i);
  }
}

char *Message::marshal()
{
  ustring mars{};
  addField(mars, MSG_TYPE_SIZE, message_type == Request ? '0' : '1');
  addField(mars, RPC_ID_SIZE, rpc_id);
  addField(mars, OP_SIZE, operation);
  addField(mars, MSG_SIZE, message_size);
  addField(mars, TOTAL_PARTS_SIZE, totalParts);
  addField(mars, PART_IDX_SIZE, partIndex);


  ustring content = base64_encode((char *)this->message, this->message_size);

  mars += content;

  if(verbose)
  {
    cout << "marshalled size = " << content.size() << endl;
    cout << "marshalled/original ratio = " << content.size()/double(message_size) << endl;
  }

  try
  {
    char *str = new char[mars.size() + 1];
    memcpy(str, mars.c_str(), mars.size() + 1);
    str[mars.size()] = 0;
    return str;
  }
  catch (exception &e)
  {
    cout << "In Message::marshal: " << e.what() << endl;
    return NULL;
  }
}

int Message::getTotal()
{
  return this->totalParts;
}

int Message::getIndex()
{
  return this->partIndex;
}

int Message::getOperation()
{
  return this->operation;
}

int Message::getRPCId()
{
  return this->rpc_id;
}

void *Message::getMessage()
{
  //---------
  void *ptr;
  try
  {
    ptr = new char[this->message_size + 1];
    memcpy((char *)ptr, (char *)this->message, this->message_size + 1);
    char *temp = (char *)ptr;
    temp[this->message_size] = 0;
    return ptr;
  }
  catch (exception &e)
  {
    cout << "In Message::getMessage: " << e.what() << endl;
    throw;
  }

  //---------
}

size_t Message::getMessageSize()
{
  return this->message_size;
}

MessageType Message::getMessageType()
{
  return this->message_type;
}

void Message::setTotal(int total)
{
  this->totalParts = total;
}

void Message::setIndex(int index)
{
  this->partIndex = index;
}

void Message::setOperation(int _operation)
{
  this->operation = _operation;
}

void Message::setMessage(const void *msg, size_t msg_size)
{
  if (this->message)
  {
    delete[](char *) this->message;
    this->message = NULL;
  }
  try
  {
    this->message = new char[msg_size + 1];
    memcpy((char *)this->message, (char *)msg, msg_size + 1);
    this->message_size = msg_size;
  }
  catch (exception &e)
  {
    cout << "In Message::setMessage: " << e.what() << endl;
    throw;
  }
}

void Message::setMessageType(MessageType msg_type)
{
  this->message_type = msg_type;
}

static bool is_base64(unsigned char c)
{
  return (isalnum(c) || (c == '+') || (c == '/'));
}

ustring Message::base64_encode(char *msg, size_t message_size)
{
  ustring encoded;
  int i = 0;
  int j = 0;
  unsigned char byte_arr_3[3];
  unsigned char byte_arr_4[4];

  size_t msgLen = message_size;
  int ctr = 0;
  while (msgLen--)
  {
    byte_arr_3[i++] = msg[ctr++];
    if (i == 3)
    {
      byte_arr_4[0] = (byte_arr_3[0] & 0xfc) >> 2;
      byte_arr_4[1] = ((byte_arr_3[0] & 0x03) << 4) + ((byte_arr_3[1] & 0xf0) >> 4);
      byte_arr_4[2] = ((byte_arr_3[1] & 0x0f) << 2) + ((byte_arr_3[2] & 0xc0) >> 6);
      byte_arr_4[3] = byte_arr_3[2] & 0x3f;

      for (i = 0; i < 4; i++)
        encoded += base64_chars[byte_arr_4[i]];
      i = 0;
    }
  }

  if (i != 0)
  {
    for (j = i; j < 3; j++)
      byte_arr_3[j] = '\0';

    byte_arr_4[0] = (byte_arr_3[0] & 0xfc) >> 2;
    byte_arr_4[1] = ((byte_arr_3[0] & 0x03) << 4) + ((byte_arr_3[1] & 0xf0) >> 4);
    byte_arr_4[2] = ((byte_arr_3[1] & 0x0f) << 2) + ((byte_arr_3[2] & 0xc0) >> 6);

    for (j = 0; j < i + 1; j++)
      encoded += base64_chars[byte_arr_4[j]];

    while ((i++ < 3))
      encoded += '=';
  }

  return encoded;
}

ustring Message::base64_decode(char *encoded_string, size_t message_size)
{

  int in_len = message_size;
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char byte_arr_4[4], byte_arr_3[3];
  ustring decoded;

  while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
  {
    byte_arr_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4)
    {
      for (i = 0; i < 4; i++)
        byte_arr_4[i] = base64_chars.find(byte_arr_4[i]);

      byte_arr_3[0] = (byte_arr_4[0] << 2) + ((byte_arr_4[1] & 0x30) >> 4);
      byte_arr_3[1] = ((byte_arr_4[1] & 0xf) << 4) + ((byte_arr_4[2] & 0x3c) >> 2);
      byte_arr_3[2] = ((byte_arr_4[2] & 0x3) << 6) + byte_arr_4[3];

      for (i = 0; i < 3; i++)
        decoded += byte_arr_3[i];
      i = 0;
    }
  }

  if (i != 0)
  {
    for (j = 0; j < i; j++)
      byte_arr_4[j] = base64_chars.find(byte_arr_4[j]);

    byte_arr_3[0] = (byte_arr_4[0] << 2) + ((byte_arr_4[1] & 0x30) >> 4);
    byte_arr_3[1] = ((byte_arr_4[1] & 0xf) << 4) + ((byte_arr_4[2] & 0x3c) >> 2);

    for (j = 0; (j < i - 1); j++)
      decoded += byte_arr_3[j];
  }

  return decoded;
}

Message::~Message()
{
  delete[](char *) this->message;
  this->message = NULL;
}

bool Message::operator==(Message *other)
{
  if (this->verbose)
  {
    cout << "Self---------------------\n";

    cout << "msg type: " << this->message_type << endl;
    cout << "Operation: " << this->operation << endl;
    cout << "RPC_ID: " << this->rpc_id << endl;
    cout << "Message Size: " << this->message_size << endl;
    cout << "Total Parts: " << this->totalParts << endl;
    cout << "Part Index: " << this->partIndex << endl;

    cout << "other: ------------------\n";

    cout << "msg type: " << other->message_type << endl;
    cout << "Operation: " << other->operation << endl;
    cout << "RPC_ID: " << other->rpc_id << endl;
    cout << "Message Size: " << other->message_size << endl;
    cout << "Total Parts: " << other->totalParts << endl;
    cout << "Part Index: " << other->partIndex << endl;
  }

  bool eq = this->operation == other->operation && this->rpc_id == other->rpc_id && this->message_size == other->message_size;
  eq = eq && this->message_type == other->message_type && this->totalParts == other->totalParts;
  eq = eq && this->partIndex == other->partIndex && (!strcmp((char *)this->message, (char *)other->message), this->message_size);
  return eq;
}
