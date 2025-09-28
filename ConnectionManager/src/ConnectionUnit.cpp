#include "ConnectionUnit.hpp"
#include "ConnectionTypes.hpp"

#include <QMetaObject>
#include <QCoreApplication>
#include <cstring>

ConnectionUnit::ConnectionUnit(const QString &ip, quint16 port, QObject *parent)
    : QObject(parent),
      listen_ip(ip),
      listen_port(port),
      running(false),
      sock(INVALID_SOCKET)
{

    start();
}

ConnectionUnit::~ConnectionUnit() {
    stop();
}




bool ConnectionUnit::initWinsock() {
    WSADATA wsaData;
    int r = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (r != 0) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("WSAStartup failed: %1").arg(r)));
        return false;
    }
    return true;
}

void ConnectionUnit::cleanupWinsock() {
    WSACleanup();
}

bool ConnectionUnit::createAndBind() {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("socket failed: %1").arg(WSAGetLastError())));
        return false;
    }

    BOOL yes = TRUE;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(listen_port));

    if (listen_ip.isEmpty() || listen_ip == "0.0.0.0") {
        addr.sin_addr.S_un.S_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, listen_ip.toStdString().c_str(), &addr.sin_addr);
    }

    if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        closesocket(sock);
        sock = INVALID_SOCKET;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("bind failed: %1").arg(err)));
        return false;
    }

    u_long mode = 0;
    ioctlsocket(sock, FIONBIO, &mode);
    qDebug() << "createAndBind ok, sock=" << sock;

    return true;
}

void ConnectionUnit::recvLoop() {
    const int BUF_SIZE = 65536;
    std::vector<char> buffer(BUF_SIZE);
    qDebug() << "starting worker thread";

    while (running.load()) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int sel = select(0, &readfds, nullptr, nullptr, &tv);
        if (sel == SOCKET_ERROR) {
            int err = WSAGetLastError();
            QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                      Q_ARG(QString, QStringLiteral("select failed: %1").arg(err)));
            break;
        } else if (sel == 0) {
            continue;
        }

        if (!FD_ISSET(sock, &readfds)) continue;

        sockaddr_in from{};
        int fromLen = sizeof(from);
        int n = recvfrom(sock, buffer.data(), BUF_SIZE, 0, reinterpret_cast<sockaddr*>(&from), &fromLen);
        if (n == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT) continue;
            QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                      Q_ARG(QString, QStringLiteral("recvfrom failed: %1").arg(err)));
            continue;
        }

        QByteArray data(buffer.data(), n);
        QHostAddress srcAddr(QString::fromStdString(inet_ntoa(from.sin_addr)));
        quint16 srcPort = ntohs(from.sin_port);
        processMessage(data,srcAddr,srcPort);
        QMetaObject::invokeMethod(this, "dataReceived", Qt::QueuedConnection,
                                  Q_ARG(QByteArray, data),
                                  Q_ARG(QHostAddress, srcAddr),
                                  Q_ARG(quint16, srcPort));
    }
}



bool ConnectionUnit::start() {
    if (running.load()) return true;
    if (!initWinsock()) return false;
    if (!createAndBind()) {
        cleanupWinsock();
        return false;
    }

    running.store(true);
    worker_thread = std::thread([this]() { recvLoop(); });

    return true;
}

void ConnectionUnit::stop() {
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false)) return;

    if (sock != INVALID_SOCKET) {
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    if (worker_thread.joinable()) worker_thread.join();
    cleanupWinsock();
}
bool ConnectionUnit::sendMessage(const QByteArray &data, const QHostAddress &addr, quint16 port) {
    if (sock == INVALID_SOCKET) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("sendTo failed: socket is invalid")));
        return false;
    }

    sockaddr_in to{};
    to.sin_family = AF_INET;
    to.sin_port = htons(static_cast<u_short>(port));

    std::string ipStr = addr.toString().toStdString();
    if (inet_pton(AF_INET, ipStr.c_str(), &to.sin_addr) != 1) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("sendTo failed: invalid IPv4 address %1").arg(QString::fromStdString(ipStr))));
        return false;
    }

    int sent = sendto(sock,
                      data.constData(),
                      static_cast<int>(data.size()),
                      0,
                      reinterpret_cast<sockaddr*>(&to),
                      sizeof(to));
    if (sent == SOCKET_ERROR) {
        int err = WSAGetLastError();
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("sendto failed: %1").arg(err)));
        return false;
    }

    return true;
}
