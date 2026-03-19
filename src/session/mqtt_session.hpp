#ifndef BROKERINO_SRC_SESSION_MQTT_SESSION_HPP
#define BROKERINO_SRC_SESSION_MQTT_SESSION_HPP

#include "core/mqtt_packet.hpp"
#include <string>
#include <unordered_map>
#include <cstdint>

namespace brokerino::core {

struct Session {
    std::string   client_id;
    bool          clean_session;
    std::uint16_t keep_alive;

    std::unordered_map<std::string, std::uint8_t>    subscriptions;
    std::unordered_map<std::uint16_t, PublishPacket> inflight;
    std::uint16_t next_packet_id = 1;
};

Session       CreateSession      (const ConnectPacket& connect);
void          AddSubscription    (Session& session,
                                  const std::string& topic_filter,
                                  std::uint8_t qos);
void          TrackInflight      (Session& session, const PublishPacket& packet);
void          AcknowledgeInflight(Session& session, std::uint16_t packet_id);
std::uint16_t NextPacketId       (Session& session);

} // namespace brokerino::core

#endif
