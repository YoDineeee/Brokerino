#ifndef BROKERINO_SRC_BROKER_BROKER_HPP_
#define BROKERINO_SRC_BROKER_BROKER_HPP_

#include "networking/tcp_server.hpp"
#include "networking/connection.hpp"
#include "core/mqtt_packet.hpp"
#include "session/mqtt_session.hpp"
#include "messaging/subscription_map.hpp"
#include "persistence/storage.hpp"
#include "persistence/recovery.hpp"
#include "auth/simple_auth.hpp"

#include <unordered_map>
#include <string>
#include <mutex>
#include <memory>
#include <vector>
#include <cstdint>

namespace brokerino {

class Broker {
public:
    explicit Broker(const std::string& db_path, int port);
    ~Broker() = default;

    Broker(const Broker&)            = delete;
    Broker& operator=(const Broker&) = delete;

    void Run();
    void Stop();

private:
    void OnClientConnected(int fd);
    void OnData            (int fd, const std::vector<std::uint8_t>& data);
    void OnDisconnect      (int fd);

    // Packet handlers
    void HandleConnect   (int fd, const std::vector<std::uint8_t>& payload);
    void HandlePublish   (int fd, const core::FixedHeader& header,
                                  const std::vector<std::uint8_t>& payload);
    void HandleSubscribe (int fd, const std::vector<std::uint8_t>& payload);
    void HandlePuback    (int fd, const std::vector<std::uint8_t>& payload);
    void HandlePingreq   (int fd);
    void HandleDisconnect(int fd);

    // Helpers
    void        SendToClient (int fd, const std::vector<std::uint8_t>& data);
    std::string ClientIdForFd(int fd) const;

    // Subsystems
    networking::TcpServer      server_;
    messaging::SubscriptionMap sub_map_;
    persistence::Storage       storage_;
    auth::SimpleAuth           auth_;

    // State
    std::unordered_map<int, std::vector<std::uint8_t>>               buffers_;
    std::unordered_map<int, core::Session>                           sessions_;
    std::unordered_map<int, std::unique_ptr<networking::Connection>> connections_;

    std::mutex mutex_;
};

} // namespace brokerino

#endif