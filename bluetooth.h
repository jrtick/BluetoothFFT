#ifndef __my_bluetooth_H_
#define __my_bluetooth_H_

typedef struct {
  int server, client;
} Connection;

Connection SetupConnection(void);
void SendMessage(Connection c, const char* msg, const int length);
void CloseConnection(Connection c);

#endif //__my_bluetooth_H_
