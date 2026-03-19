#ifndef BROKERINO_SRC_NETWORKING_CONNECTION_HPP
#define BROKERINO_SRC_NETWORKING_CONNECTION_HPP

#include <cstdint>
#include <functional>
#include <vector>

namespace brokerino::networking {

using DataReceivedCallback       = std::function<void(const std::vector<std::uint8_t>&)>;
using ClientDisconnectedCallback = std::function<void()>;

class Connection {
public:
    static constexpr std::size_t kReadBufferSize = 4096;

    explicit Connection(int fd);
    ~Connection();

    Connection(const Connection&)            = delete;
    Connection& operator=(const Connection&) = delete;

    void Run (DataReceivedCallback on_data,
              ClientDisconnectedCallback on_disconnect);
    void Send(const std::vector<std::uint8_t>& data);
    void Close();
    bool IsConnected() const;
    int  GetFd()       const;

private:
    int  fd_;
    bool connected_;
};

} // namespace brokerino::networking

#endif
