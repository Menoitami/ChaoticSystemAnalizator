#include "FrontendBase.hpp"
#include <QDebug>
#include <QMetaObject>
#include <QCoreApplication>
#include <vector>

FrontendBase::FrontendBase(const QString& ip, quint16 port, QObject* parent)
    : QObject(parent),
      server_sock(INVALID_SOCKET),
      running(false),
      listen_ip(ip),
      listen_port(port)
{
}

FrontendBase::~FrontendBase() {
    stop();
}

bool FrontendBase::initWinsock() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        emit errorOccurred(QStringLiteral("WSAStartup failed: %1").arg(iResult));
        return false;
    }
    return true;
}

void FrontendBase::cleanupWinsock() {
    WSACleanup();
}

bool FrontendBase::createAndBind() {
    struct addrinfo hints{};
    struct addrinfo* result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    std::string ipStd = listen_ip.toStdString();
    std::string portStr = std::to_string(listen_port);

    int rv = getaddrinfo(ipStd.c_str(), portStr.c_str(), &hints, &result);
    if (rv != 0) {
        emit errorOccurred(QStringLiteral("getaddrinfo failed: %1").arg(rv));
        return false;
    }

    server_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_sock == INVALID_SOCKET) {
        emit errorOccurred(QStringLiteral("socket failed: %1").arg(WSAGetLastError()));
        freeaddrinfo(result);
        return false;
    }

    BOOL yes = TRUE;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

    if (bind(server_sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        emit errorOccurred(QStringLiteral("bind failed: %1").arg(WSAGetLastError()));
        closesocket(server_sock);
        server_sock = INVALID_SOCKET;
        freeaddrinfo(result);
        return false;
    }

    freeaddrinfo(result);

    if (listen(server_sock, SOMAXCONN) == SOCKET_ERROR) {
        emit errorOccurred(QStringLiteral("listen failed: %1").arg(WSAGetLastError()));
        closesocket(server_sock);
        server_sock = INVALID_SOCKET;
        return false;
    }

    return true;
}

void FrontendBase::listenLoop() {
    while (running.load()) {
        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);
        SOCKET clientSock = accept(server_sock, (sockaddr*)&clientAddr, &addrLen);
        if (clientSock == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (!running.load()) break;
            emit errorOccurred(QStringLiteral("accept failed: %1").arg(err));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        if (currentClientSock != INVALID_SOCKET) {
            shutdown(currentClientSock, SD_BOTH);
            closesocket(currentClientSock);
            currentClientSock = INVALID_SOCKET;
        }
        currentClientSock = clientSock;

        QMetaObject::invokeMethod(this, "connected", Qt::QueuedConnection);
        QtConcurrent::run([this, clientSock]() {
            this->handleClientSocket(clientSock);
        });
    }

           // Закрываем server_sock на выходе
    if (server_sock != INVALID_SOCKET) {
        shutdown(server_sock, SD_BOTH);
        closesocket(server_sock);
        server_sock = INVALID_SOCKET;
    }
}

void FrontendBase::handleClientSocket(SOCKET clientSock) {
    int timeoutMs = 2000;
    setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));

    const int chunkSize = 4096;
    std::vector<char> tempBuf(chunkSize);
    std::vector<char> allData;
    while (true) {
        int n = recv(clientSock, tempBuf.data(), chunkSize, 0);
        if (n > 0) {
            allData.insert(allData.end(), tempBuf.data(), tempBuf.data() + n);
            continue;
        } else if (n == 0) {
            break;
        } else {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT) {
                break;
            } else {
                QMetaObject::invokeMethod(this, "errorOccurred",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, QStringLiteral("recv error: %1").arg(err)));
                break;
            }
        }
    }

    shutdown(clientSock, SD_BOTH);
    closesocket(clientSock);
    if (currentClientSock == clientSock) {
        currentClientSock = INVALID_SOCKET;
    }

    QByteArray bytes(reinterpret_cast<const char*>(allData.data()), static_cast<int>(allData.size()));
    QMetaObject::invokeMethod(this, "dataReceived", Qt::QueuedConnection, Q_ARG(QByteArray, bytes));
}



bool FrontendBase::start() {
    if (running.load()) return true;

    if (!initWinsock()) return false;
    if (!createAndBind()) {
        cleanupWinsock();
        return false;
    }

    running.store(true);
    listener_thread = std::thread([this]() { this->listenLoop(); });
    return true;
}

void FrontendBase::stop() {
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false)) return;
    if (server_sock != INVALID_SOCKET) {
        shutdown(server_sock, SD_BOTH);
        closesocket(server_sock);
        server_sock = INVALID_SOCKET;
    }
    if (currentClientSock != INVALID_SOCKET) {
        shutdown(currentClientSock, SD_BOTH);
        closesocket(currentClientSock);
        currentClientSock = INVALID_SOCKET;
    }

    if (listener_thread.joinable()) listener_thread.join();
    cleanupWinsock();
}

bool FrontendBase::sendMessage(const QByteArray &data) {
    SOCKET target = currentClientSock;
    if (target == INVALID_SOCKET) {
        QMetaObject::invokeMethod(this, "errorOccurred",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("No client connected")));
        return false;
    }

    QtConcurrent::run([this, target, data]() {
        const char *buf = data.constData();
        int total = data.size();
        int sent = 0;
        while (sent < total) {
            int n = send(target, buf + sent, total - sent, 0);
            if (n > 0) {
                sent += n;
                continue;
            } else if (n == 0) {
                QMetaObject::invokeMethod(this, "errorOccurred",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, QStringLiteral("Connection closed during send")));
                break;
            } else {
                int err = WSAGetLastError();
                QMetaObject::invokeMethod(this, "errorOccurred",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, QStringLiteral("send failed: %1").arg(err)));
                break;
            }
        }
    });

    return true;
}
