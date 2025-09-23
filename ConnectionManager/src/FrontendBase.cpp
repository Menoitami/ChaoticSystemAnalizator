#include "FrontendBase.hpp"

FrontendBase::FrontendBase(int port) : port(port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_sock, SOMAXCONN);
    std::cout << "Frontend is waiting for a connection on port " << port << std::endl;
}

void FrontendBase::waitForConnection() {
    SOCKET client_sock = accept(server_sock, nullptr, nullptr);
    if (client_sock == INVALID_SOCKET) {
        perror("Accept failed");
        return;
    }

    char buffer[1024] = {0};
    recv(client_sock, buffer, sizeof(buffer), 0);
    std::cout << "Frontend received: " << buffer << std::endl;

    const char* response = "Hello from Frontend!";
    send(client_sock, response, strlen(response), 0);
    closesocket(client_sock);
    closesocket(server_sock);
    WSACleanup();
}
