#include "BackendBase.hpp"

BackendBase::BackendBase(const std::string& ip, int port) : ip(ip), port(port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr);

           // Подключение к Frontend
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
}

void BackendBase::start() {
    const char* message = "Hello from BackendBase!";
    send(sock, message, strlen(message), 0);
    closesocket(sock);
    WSACleanup();
}
