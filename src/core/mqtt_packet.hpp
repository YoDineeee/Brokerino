#ifndef BROKERINO_SRC_CORE_MQTT_PACKET_HPP_
#define BROKERINO_SRC_CORE_MQTT_PACKET_HPP_

#include "core/mqtt_codec.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace brokerino::core {

// CONNECT 
struct ConnectPacket {
    std::string   client_id;
    std::string   username;
    std::string   password;
    std::uint16_t keep_alive;
    bool          clean_session;
};

//  CONNACK 
enum class ConnectReturnCode : std::uint8_t {
    Accepted                 = 0,
    RefusedBadProtocol       = 1,
    RefusedClientIdRejected  = 2,
    RefusedServerUnavailable = 3,
    RefusedBadCredentials    = 4,
    RefusedNotAuthorized     = 5,
};

struct ConnackPacket {
    bool              session_present;
    ConnectReturnCode return_code;
};

// PUBLISH 
struct PublishPacket {
    std::string               topic;
    std::vector<std::uint8_t> payload;
    std::uint8_t              qos;
    bool                      retain;
    bool                      dup;
    std::optional<std::uint16_t> packet_id;
};

// PUBACK 
struct PubackPacket {
    std::uint16_t packet_id;
};

//  SUBSCRIBE 
struct Subscription {
    std::string  topic_filter;
    std::uint8_t qos;
};

struct SubscribePacket {
    std::uint16_t             packet_id;
    std::vector<Subscription> subscriptions;
};

// SUBACK 
struct SubackPacket {
    std::uint16_t             packet_id;
    std::vector<std::uint8_t> return_codes;
};

// Parse functions 
std::optional<ConnectPacket>   ParseConnect  (const std::vector<std::uint8_t>& payload);
std::optional<PublishPacket>   ParsePublish  (const FixedHeader& header,
                                              const std::vector<std::uint8_t>& payload);
std::optional<PubackPacket>    ParsePuback   (const std::vector<std::uint8_t>& payload);
std::optional<SubscribePacket> ParseSubscribe(const std::vector<std::uint8_t>& payload);

//  Serialize functions 
std::vector<std::uint8_t> SerializeConnack (const ConnackPacket& packet);
std::vector<std::uint8_t> SerializePublish (const PublishPacket& packet);
std::vector<std::uint8_t> SerializePuback  (const PubackPacket& packet);
std::vector<std::uint8_t> SerializeSuback  (const SubackPacket& packet);
std::vector<std::uint8_t> SerializePingresp();

} // namespace brokerino::core

#endif