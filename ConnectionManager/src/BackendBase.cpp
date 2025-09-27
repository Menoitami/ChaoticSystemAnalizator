#include "BackendBase.hpp"
#include <QByteArray>
#include <QDebug>
#include <thread>
#include <atomic>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

BackendBase::BackendBase(const QString &destIp, quint16 destPort, QObject *parent)
    : QObject(parent),
      dest_ip(destIp),
      dest_port(destPort),
      running(false),
      sock(INVALID_SOCKET)
{
    ZeroMemory(&destAddr, sizeof(destAddr));
}

BackendBase::~BackendBase()
{
    stop();
    cleanupWinsock();
}

bool BackendBase::initWinsock()
{
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (res != 0) {
        emit errorOccurred(QString("WSAStartup failed: %1").arg(res));
        return false;
    }
    return true;
}

void BackendBase::cleanupWinsock()
{
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
    WSACleanup();
}

bool BackendBase::createSocket()
{
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        emit errorOccurred(QString("socket() failed: %1").arg(WSAGetLastError()));
        return false;
    }

    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(dest_port);
    // prefer inet_pton, but use inet_addr for simplicity
    destAddr.sin_addr.s_addr = inet_addr(dest_ip.toStdString().c_str());
    if (destAddr.sin_addr.s_addr == INADDR_NONE) {
        // try resolving via getaddrinfo
        sockaddr_in tmp = {};
        tmp.sin_family = AF_INET;
        if (inet_pton(AF_INET, dest_ip.toStdString().c_str(), &tmp.sin_addr) != 1) {
            emit errorOccurred(QString("Invalid destination IP: %1").arg(dest_ip));
            return false;
        }
        destAddr.sin_addr = tmp.sin_addr;
    }
    return true;
}

bool BackendBase::sendOnce(const QByteArray &data)
{
    if (sock == INVALID_SOCKET) {
        emit errorOccurred("Socket is not created");
        return false;
    }

    int sent = sendto(sock,
                      reinterpret_cast<const char*>(data.constData()),
                      static_cast<int>(data.size()),
                      0,
                      reinterpret_cast<sockaddr*>(&destAddr),
                      sizeof(destAddr));
    if (sent == SOCKET_ERROR) {
        emit errorOccurred(QString("sendto() failed: %1").arg(WSAGetLastError()));
        return false;
    }
    return true;
}

bool BackendBase::start()
{
    if (running.exchange(true)) {
        return true;
    }

    if (!initWinsock()) {
        running = false;
        return false;
    }

    if (!createSocket()) {
        cleanupWinsock();
        running = false;
        return false;
    }

    QByteArray msg = "Hello from BackendBase";
    worker_thread = std::thread([this, msg]() {
        const int maxAttempts = 3;
        for (int i = 0; i < maxAttempts && running; ++i) {
            if (sendOnce(msg)) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    qDebug() << "message_send";
    return true;
}

void BackendBase::stop()
{
    if (!running.exchange(false)) {
        return;
    }

    if (worker_thread.joinable()) {
        worker_thread.join();
    }

    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    cleanupWinsock();
}
