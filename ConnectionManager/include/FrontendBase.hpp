#pragma once

#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>

class FrontendBase {
  public:
    FrontendBase(int port);
    void waitForConnection();

  private:
    SOCKET server_sock;
    int port;
};

