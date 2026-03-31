#include "comm_server.h"
#include <iostream>
#include <vector>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
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
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
}

void CommServer::server_loop() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return;
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) {
        perror("setsockopt");
        return;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        return;
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return;
    }

    std::cout << "CommServer listening on port " << port_ << std::endl;

    while (running_.load()) {
        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            // Check if the server is still running, if not, it's a graceful shutdown
            if (!running_.load()) break;
            perror("accept");
            continue;
        }
        std::cout << "Connection accepted from GUI tuner.\n";

        // Read data from the client
        int valread = read(new_socket, buffer, 1024);
        if (valread > 0) {
            buffer[valread] = '\0'; // Null-terminate the received data
            data_handler_(std::string(buffer));
        }

        // Close the client socket
#ifdef _WIN32
        closesocket(new_socket);
#else
        close(new_socket);
#endif
    }

    // Close the server socket
#ifdef _WIN32
    closesocket(server_fd);
#else
    close(server_fd);
#endif
    std::cout << "CommServer stopped.\n";
}
