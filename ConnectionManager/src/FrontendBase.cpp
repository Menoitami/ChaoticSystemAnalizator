#include "FrontendBase.hpp"

FrontendBase::FrontendBase(int port) : port(port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        std::cerr << "socket error: " << WSAGetLastError() << "\n";
        exit(1);
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);

    if (bind(server_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "bind error: " << WSAGetLastError() << "\n";
        closesocket(server_sock);
        WSACleanup();
        exit(1);
    }

    if (listen(server_sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen error: " << WSAGetLastError() << "\n";
        closesocket(server_sock);
        WSACleanup();
        exit(1);
    }

    std::cout << "Frontend listening on 127.0.0.1:" << port << "\n";
}

FrontendBase::~FrontendBase() {
    if (server_sock != INVALID_SOCKET) closesocket(server_sock);
    WSACleanup();
    std::cout << "Frontend stopped\n";
}

void FrontendBase::handleClient(SOCKET client_sock) {
    char buffer[1024];
    while (true) {
        int rec = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (rec == SOCKET_ERROR) {
            std::cerr << "recv error: " << WSAGetLastError() << "\n";
            break;
        } else if (rec == 0) {
            std::cout << "Client disconnected\n";
            break;
        } else {
            buffer[rec] = '\0';
            std::cout << "Received (" << rec << "): " << buffer << "\n";

            const char* response = "Hello from Frontend!";
            int sent = send(client_sock, response, (int)strlen(response), 0);
            if (sent == SOCKET_ERROR) {
                std::cerr << "send error: " << WSAGetLastError() << "\n";
                break;
            }
        }
    }
    closesocket(client_sock);
}

void FrontendBase::run() {
    while (true) {
        SOCKET client_sock = accept(server_sock, nullptr, nullptr);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "accept error: " << WSAGetLastError() << "\n";
            // при критичной ошибке можно break; иначе continue
            continue;
        }
        std::cout << "Client connected: " << client_sock << "\n";
        handleClient(client_sock);
        // после обработки вернёмся в accept для новых подключений
    }
}
