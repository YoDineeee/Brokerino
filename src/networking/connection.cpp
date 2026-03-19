#include "networking/connection.hpp"

#include <sys/socket.h>
#include <unistd.h>

namespace brokerino::networking {

Connection::Connection(int fd)
    : fd_(fd)
    , connected_(fd != -1) {}

Connection::~Connection() {
    Close();
}

void Connection::Run(DataReceivedCallback on_data,
                     ClientDisconnectedCallback on_disconnect) {
    std::vector<std::uint8_t> buf(kReadBufferSize);

    while (connected_) {
        ssize_t bytes = recv(fd_, buf.data(), buf.size(), 0);
        if (bytes <= 0) break;
        on_data({buf.begin(), buf.begin() + bytes});
    }

    Close();
    on_disconnect();
}

void Connection::Send(const std::vector<std::uint8_t>& data) {
    if (!connected_) return;
    send(fd_, data.data(), data.size(), 0);
}

void Connection::Close() {
    if (fd_ != -1) {
        close(fd_);
        fd_        = -1;
        connected_ = false;
    }
}

bool Connection::IsConnected() const { return connected_; }
int  Connection::GetFd()        const { return fd_; }

} // namespace brokerino::networking
