/**
 * @file mqtt_session.hpp
 * @brief MQTT session management (in-memory representation).
 *
 * A Session holds per-client state: subscriptions, inflight QoS messages,
 * and packet ID counter. Used heavily by the Broker and Persistence layers.
 */


#ifndef BROKERINO_SRC_SESSION_MQTT_SESSION_HPP
#define BROKERINO_SRC_SESSION_MQTT_SESSION_HPP

#include "core/mqtt_packet.hpp"
#include <string>
#include <unordered_map>
#include <cstdint>

namespace brokerino::core {

    /**
 * @struct Session
 * @brief Represents a client's MQTT session state.
 */


struct Session {
    std::string   client_id;  ///< Unique client identifier
    bool          clean_session; ///< Clean Session flag from CONNECT
    std::uint16_t keep_alive;  ///< Keep-alive interval

    std::unordered_map<std::string, std::uint8_t>    subscriptions;  ///< Topic filter → QoS
    std::unordered_map<std::uint16_t, PublishPacket> inflight; ///< QoS 1 messages waiting for PUBACK
    std::uint16_t next_packet_id = 1;  ///< Packet ID generator for QoS 1
};

/** @name Session Helper Functions */
//@{
Session       CreateSession      (const ConnectPacket& connect);
void          AddSubscription    (Session& session,
                                  const std::string& topic_filter,
                                  std::uint8_t qos);
void          TrackInflight      (Session& session, const PublishPacket& packet);
void          AcknowledgeInflight(Session& session, std::uint16_t packet_id);
std::uint16_t NextPacketId       (Session& session);
//@}

} // namespace brokerino::core

#endif
