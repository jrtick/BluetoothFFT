#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "bluetooth.h"

Connection SetupConnection() {
  //allocate socket
  int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  //bind socket to port 1 of first available adapter
  struct sockaddr_rc loc_addr = {0};
  loc_addr.rc_family = AF_BLUETOOTH;
  loc_addr.rc_bdaddr = *BDADDR_ANY;
  loc_addr.rc_channel = 1;
  bind (s, (const struct sockaddr*) &loc_addr, sizeof(loc_addr));

  //put socket into listening mode
  listen(s, 1);

  //accept only one connection
  printf("Waiting for connection...\n");
  struct sockaddr_rc rem_addr = {0};
  unsigned int opt = sizeof(rem_addr);
  int client = accept(s, (struct sockaddr*) &rem_addr, &opt);
  printf("Connected!! ");
  fflush(stdout);

  char buf[1024] = {0};
  ba2str(&rem_addr.rc_bdaddr, buf);
  printf("Accepted connection from %s\n", buf);
  memset(buf, 0, sizeof(buf));

  Connection c;
  c.server = s;
  c.client = client;
  return c;
}

void SendMessage(Connection c, const char* msg, const int length) {
  int offset = 0;
  while(offset != length) {
    offset += send(c.client, msg+offset, length-offset, 0);
  }
}

void CloseConnection(Connection c) {
  close(c.client);
  close(c.server);
}
