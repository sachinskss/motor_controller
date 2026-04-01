#include "comm_server.h"
#include <iostream>
#include <vector>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
#endif

CommServer::CommServer(int port, std::function<void(const std::string&)> data_handler)
    : port_(port), data_handler_(std::move(data_handler)), running_(false) {
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed.\n";
    }
#endif
}

CommServer::~CommServer() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void CommServer::start() {
    if (running_.load()) {
        std::cout << "Server already running.\n";
        return;
    }
    running_.store(true);
    server_thread_ = std::thread(&CommServer::server_loop, this);
}

void CommServer::stop() {
    if (!running_.load()) {
        return;
    }
    running_.store(false);

#ifdef _WIN32
    // On Windows, you might need to close the socket to unblock accept()
    // This is a simple way to interrupt the blocking call.
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET) {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
        connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        closesocket(sock);
    }
#endif

    if (server_thread_.joinable()) {
        server_thread_.join();
    }
}

void CommServer::server_loop() {
#ifdef _WIN32
    SOCKET server_fd = INVALID_SOCKET;
    SOCKET new_socket = INVALID_SOCKET;
#else
    int server_fd = -1;
    int new_socket = -1;
#endif

    struct sockaddr_in address;
    int opt = 1;
    char buffer[1024] = {0};

    #ifdef _WIN32
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
#else
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
#endif
        std::cerr << "socket failed" << std::endl;
        return;
    }

#ifdef _WIN32
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        std::cerr << "setsockopt failed" << std::endl;
        closesocket(server_fd);
        return;
    }
#else
    // SO_REUSEPORT is not standard on all systems, SO_REUSEADDR is more portable
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        return;
    }
#endif

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
#ifdef _WIN32
        closesocket(server_fd);
#endif
        return;
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
#ifdef _WIN32
        closesocket(server_fd);
#endif
        return;
    }

    std::cout << "CommServer listening on port " << port_ << std::endl;

    while (running_.load()) {
#ifdef _WIN32
        int addrlen = sizeof(address);
#else
        socklen_t addrlen = sizeof(address);
#endif
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

#ifdef _WIN32
        if (new_socket == INVALID_SOCKET) {
            if (!running_.load()) break; // Graceful shutdown
            std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }
#else
        if (new_socket < 0) {
            if (!running_.load()) break;
            perror("accept");
            continue;
        }
#endif

        std::cout << "Connection accepted from GUI tuner.\n";

#ifdef _WIN32
        int valread = recv(new_socket, buffer, 1024, 0);
#else
        int valread = read(new_socket, buffer, 1024);
#endif
        if (valread > 0) {
            buffer[valread] = '\0';
            data_handler_(std::string(buffer));
        }

#ifdef _WIN32
        closesocket(new_socket);
#else
        close(new_socket);
#endif
    }

#ifdef _WIN32
    if (server_fd != INVALID_SOCKET) closesocket(server_fd);
#else
    if (server_fd != -1) close(server_fd);
#endif
    std::cout << "CommServer stopped.\n";
}
