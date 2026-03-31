#ifndef COMM_SERVER_H
#define COMM_SERVER_H

#include <string>
#include <functional>
#include <thread>
#include <atomic>

class CommServer {
public:
    CommServer(int port, std::function<void(const std::string&)> data_handler);
    ~CommServer();

    void start();
    void stop();

private:
    int port_;
    std::function<void(const std::string&)> data_handler_;
    std::thread server_thread_;
    std::atomic<bool> running_;

    void server_loop();
};

#endif // COMM_SERVER_H
