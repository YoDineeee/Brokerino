/**
 * @file mqtt_packet.hpp
 * @brief MQTT 3.1.1 packet definitions and codec interface.
 *
 * This file defines all MQTT control packet structures and provides
 * parsing and serialization functions according to the MQTT 3.1.1 specification.
 */





#ifndef BROKERINO_SRC_CORE_MQTT_PACKET_HPP
#define BROKERINO_SRC_CORE_MQTT_PACKET_HPP

#include "core/mqtt_codec.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace brokerino::core {

// CONNECT 


/**
 * @brief Parsed CONNECT packet sent by a client.
 */

struct ConnectPacket {
    std::string   client_id;  ///< Client identifier
    std::string   username;   ///< Optional username
    std::string   password;   ///< Optional password
    std::uint16_t keep_alive;  ///< Keep alive interval in seconds
    bool          clean_session;  ///< Clean Session flag
};


//  CONNACK 

/**
 * @brief Return codes for CONNACK packet.
 */


enum class ConnectReturnCode : std::uint8_t {
    Accepted                 = 0,
    RefusedBadProtocol       = 1,
    RefusedClientIdRejected  = 2,
    RefusedServerUnavailable = 3,
    RefusedBadCredentials    = 4,
    RefusedNotAuthorized     = 5,
};

/**
 * @brief CONNACK packet sent by the broker.
 */


struct ConnackPacket {
    bool              session_present; ///< Whether a previous session was restored
    ConnectReturnCode return_code;   ///< Connection result
};

// PUBLISH 

/**
 * @brief PUBLISH packet (sent by client or broker).
 */

struct PublishPacket {
    std::string               topic;  ///< Topic name
    std::vector<std::uint8_t> payload;  ///< Message payload
    std::uint8_t              qos;  ///< Quality of Service (0 or 1)
    bool                      retain;  ///< Retain flag
    bool                      dup;  //< Duplicate flag
    std::optional<std::uint16_t> packet_id;  ///< Packet identifier (for QoS > 0)
};

// PUBACK 

/**
 * @brief PUBACK packet (QoS 1 acknowledgment).
 */


struct PubackPacket {
    std::uint16_t packet_id;
};

//  SUBSCRIBE 
/**
 * @brief Single subscription entry.
 */

struct Subscription {
    std::string  topic_filter; ///< Topic filter (may contain + or #)
    std::uint8_t qos;  ///< Requested QoS
};

/**
 * @brief SUBSCRIBE packet from client.
 */


struct SubscribePacket {
    std::uint16_t             packet_id;
    std::vector<Subscription> subscriptions;
};

/**
 * @brief SUBACK packet returned by broker.
 */
 

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