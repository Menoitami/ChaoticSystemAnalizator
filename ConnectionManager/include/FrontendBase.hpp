#pragma once

#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>

class FrontendBase {
  public:
    FrontendBase(int port);
    ~FrontendBase();
    void run(); // запускает основной цикл (вместо waitForConnection)
  private:
    int port;
    SOCKET server_sock = INVALID_SOCKET;
    void handleClient(SOCKET client_sock);
};

