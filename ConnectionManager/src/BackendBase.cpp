#include "BackendBase.hpp"


#include <iostream>
#include <chrono>
#include <algorithm>

BackendBase::BackendBase(const std::string& ip, uint16_t port)
    : serverSock(INVALID_SOCKET),
      currentClientSock(INVALID_SOCKET),
      running(false),
      listenIp(ip),
      listenPort(port)
{
}

BackendBase::~BackendBase() {
    stop();
}

bool BackendBase::initWinsock() {
    WSADATA wsaData;
    int r = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (r != 0) {
        reportError("WSAStartup failed: " + std::to_string(r));
        return false;
    }
    return true;
}

void BackendBase::cleanupWinsock() {
    WSACleanup();
}

bool BackendBase::createAndBind() {
    struct addrinfo hints{};
    struct addrinfo* result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    std::string portStr = std::to_string(listenPort);
    int rv = getaddrinfo(listenIp.c_str(), portStr.c_str(), &hints, &result);
    if (rv != 0) {
        reportError("getaddrinfo failed: " + std::to_string(rv));
        return false;
    }

    serverSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serverSock == INVALID_SOCKET) {
        reportError("socket failed: " + std::to_string(WSAGetLastError()));
        freeaddrinfo(result);
        return false;
    }

    BOOL yes = TRUE;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

    if (bind(serverSock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        reportError("bind failed: " + std::to_string(WSAGetLastError()));
        closesocket(serverSock);
        serverSock = INVALID_SOCKET;
        freeaddrinfo(result);
        return false;
    }

    freeaddrinfo(result);

    if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) {
        reportError("listen failed: " + std::to_string(WSAGetLastError()));
        closesocket(serverSock);
        serverSock = INVALID_SOCKET;
        return false;
    }

    return true;
}

bool BackendBase::start() {
    if (running.load()) return true;
    if (!initWinsock()) return false;
    if (!createAndBind()) {
        cleanupWinsock();
        return false;
    }

    running.store(true);
    listenerThread = std::thread(&BackendBase::listenLoop, this);
    return true;
}

void BackendBase::stop() {
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false)) {
        // already stopped
    }

           // Закрываем серверный сокет чтобы разблокировать accept
    if (serverSock != INVALID_SOCKET) {
        shutdown(serverSock, SD_BOTH);
        closesocket(serverSock);
        serverSock = INVALID_SOCKET;
    }

           // Закрываем клиентский сокет, если есть
    SOCKET client = currentClientSock.load();
    if (client != INVALID_SOCKET) {
        shutdown(client, SD_BOTH);
        closesocket(client);
        currentClientSock.store(INVALID_SOCKET);
    }

    if (listenerThread.joinable()) listenerThread.join();

    cleanupWinsock();
}

void BackendBase::listenLoop() {
    while (running.load()) {
        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);
        SOCKET clientSock = accept(serverSock, (sockaddr*)&clientAddr, &addrLen);
        if (clientSock == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (!running.load()) break;
            reportError("accept failed: " + std::to_string(err));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

               // Если уже был клиент — закрываем его и заменяем
        SOCKET prev = currentClientSock.exchange(clientSock);
        if (prev != INVALID_SOCKET && prev != clientSock) {
            shutdown(prev, SD_BOTH);
            closesocket(prev);
        }

               // Обработка клиента в том же потоке listener (последовательно),
               // либо можно создавать отдельный рабочий поток. Здесь читаем и затем возвращаемся к accept.
        handleClientSocket(clientSock);

               // После обработки handleClientSocket закроет clientSock и сбросит currentClientSock если нужно
    }

           // очистка при выходе
    if (serverSock != INVALID_SOCKET) {
        shutdown(serverSock, SD_BOTH);
        closesocket(serverSock);
        serverSock = INVALID_SOCKET;
    }
}

void BackendBase::handleClientSocket(SOCKET clientSock) {
    // Установим таймаут recv (мс)
    int timeoutMs = 2000;
    setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));

    const int chunkSize = 4096;
    std::vector<uint8_t> buffer;
    buffer.reserve(4096);
    std::vector<uint8_t> temp(chunkSize);

    while (running.load()) {
        int n = recv(clientSock, reinterpret_cast<char*>(temp.data()), chunkSize, 0);
        if (n > 0) {
            buffer.insert(buffer.end(), temp.begin(), temp.begin() + n);
            continue;
        } else if (n == 0) {
            // клиент закрыл соединение
            break;
        } else {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT) {
                // предполагаем, что передача завершена
                break;
            } else {
                reportError("recv error: " + std::to_string(err));
                break;
            }
        }
    }

           // Закрываем клиентский сокет
    shutdown(clientSock, SD_BOTH);
    closesocket(clientSock);

           // Если currentClientSock указывает на этот сокет — сбрасываем
    currentClientSock.compare_exchange_strong(clientSock, INVALID_SOCKET);

           // Вызываем колбэк с полученными данными (если установлен)
    if (dataCb) {
        try {
            dataCb(buffer);
        } catch (...) {
            // подавляем исключения из колбэка
        }
    }
}

bool BackendBase::sendMessage(const std::vector<uint8_t>& data) {
    SOCKET target = currentClientSock.load();
    if (target == INVALID_SOCKET) {
        reportError("sendMessage: no client connected");
        return false;
    }

    int total = static_cast<int>(data.size());
    const char* buf = reinterpret_cast<const char*>(data.data());
    int sent = 0;
    while (sent < total) {
        int n = send(target, buf + sent, total - sent, 0);
        if (n > 0) {
            sent += n;
            continue;
        } else if (n == 0) {
            reportError("send returned 0 (connection closed)");
            return false;
        } else {
            int err = WSAGetLastError();
            reportError("send failed: " + std::to_string(err));
            return false;
        }
    }
    return true;
}

void BackendBase::reportError(const std::string& s) {
    if (errorCb) {
        try { errorCb(s); } catch(...) {}
    } else {
        // По умолчанию печатаем в stderr
        std::cerr << "BackendBase error: " << s << std::endl;
    }
}
