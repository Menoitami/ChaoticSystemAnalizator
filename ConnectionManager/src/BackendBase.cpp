#include "BackendBase.hpp"

BackendBase::BackendBase(const std::string& ip, int port) : ip(ip), port(port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { perror("socket"); exit(1); }

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(1);
    }

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "connect error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "connect succeeded\n";
    }
}

void BackendBase::start() {
    const char* message = "Hello from BackendBase!";
    int sent = send(sock, message, (int)strlen(message), 0);
    if (sent == SOCKET_ERROR) std::cerr << "send error: " << WSAGetLastError() << "\n";
    else std::cout << "Backend sent: " << sent << " bytes\n";
    closesocket(sock);
    WSACleanup();
}
