#ifndef BROKERINO_SRC_NETWORKING_TCP_SERVER_HPP
#define BROKERINO_SRC_NETWORKING_TCP_SERVER_HPP

#include <functional>
#include <string>

namespace brokerino::networking {

using ClientConnectedCallBack = std::function<void(int fd)>;

class TcpServer {
public:
    static constexpr int kDefaultPort = 1883;
    static constexpr int kBacklog     = 10;

    explicit TcpServer(int port = kDefaultPort);
    ~TcpServer();

    TcpServer(const TcpServer&)            = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    void Run(ClientConnectedCallBack on_connect);
    void Stop();
    bool IsRunning() const;
    int  GetPort()   const;

private:
    void SetupSocket();

    int  port_;
    int  server_fd_;
    bool is_running_ = false;
};

} // namespace brokerino::networking

#endif
