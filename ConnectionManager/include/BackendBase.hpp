#pragma once

#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>

class BackendBase {
  public:
    BackendBase(const std::string& ip, int port);
    void start();

  private:
    SOCKET sock;
    std::string ip;
    int port;
};

