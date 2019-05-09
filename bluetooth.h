#ifndef __my_bluetooth_H_
#define __my_bluetooth_H_

void SetupConnection(void);
void SendMessage(const char* msg, const int length);
void CloseConnection(void);

#endif //__my_bluetooth_H_
