#pragma once

#include <functional>
#include <iostream>
#include <thread>
#include <atomic>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>

class BackendBase {
  public:
    using DataCallback = std::function<void(const std::vector<uint8_t>&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    BackendBase(const std::string& ip = "127.0.0.1", uint16_t port = 8080);
    ~BackendBase();

           // Запуск/остановка
    bool start();
    void stop();
    bool isRunning() const { return running.load(); }

    bool sendMessage(const std::vector<uint8_t>& data);

    void setDataCallback(DataCallback cb) { dataCb = std::move(cb); }
    void setErrorCallback(ErrorCallback cb) { errorCb = std::move(cb); }

  private:
    // internals
    SOCKET serverSock;
    std::atomic<SOCKET> currentClientSock; // atomic чтобы чтение/запись были относительно безопасны
    std::atomic<bool> running;
    std::thread listenerThread;

    std::string listenIp;
    uint16_t listenPort;

    DataCallback dataCb;
    ErrorCallback errorCb;

    bool initWinsock();
    void cleanupWinsock();
    bool createAndBind();
    void listenLoop();
    void handleClientSocket(SOCKET clientSock);
    void reportError(const std::string& s);
};
