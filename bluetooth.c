#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "bluetooth.h"

static int server = -1;
static int client = -1;

void SetupConnection() {
  if(server == -1) {
    //allocate socket
    int enable = 1;
    server = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    //bind socket to port 1 of first available adapter
    struct sockaddr_rc loc_addr = {0};
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = 1;
    bind (server, (const struct sockaddr*) &loc_addr, sizeof(loc_addr));

    //put socket into listening mode
    listen(server, 1);
  }

  //accept only one connection
  printf("Waiting for connection...\n");
  struct sockaddr_rc rem_addr = {0};
  unsigned int opt = sizeof(rem_addr);
  client = accept(server, (struct sockaddr*) &rem_addr, &opt);
  printf("Connected!! ");
  fflush(stdout);

  char buf[1024] = {0};
  ba2str(&rem_addr.rc_bdaddr, buf);
  printf("Accepted connection from %s\n", buf);
  memset(buf, 0, sizeof(buf));
}

void SendMessage(const char* msg, const int length) {
  if(client == -1) return;

  int offset = 0;
  while(offset != length) {
    int retval = send(client, msg+offset, length-offset, 0);
    if(retval < 0) {
      // client disconnected
      client = -1;
      printf("client disconnected!!\n");
      SetupConnection(); // wait 'til another client connects before returning
      return;
    } else offset += retval;
  }
}

void CloseConnection() {
  if(client) close(client);
  if(server) close(server);
}
