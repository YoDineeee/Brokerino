//the mqtt_packet takes raw bytes after the fixed  header and parse the actual content ( like the payload , ClientID, topic)

#ifndef BROKERINO_SRC_CORE_MQTT_PACKET_HPP_
#define BROKERINO_SRC_CORE_MQTT_PACKET_HPP_

#include "core/mqtt_codec.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include<optional>

namespace brokerino::core{
    //connect
    struct ConnectPacket{
        std::string client_id;
        std::string username;
        std::string  password;
        std::uint16_t keep_alive; // in seconds 

        bool clean_session ;

    }

    //connack

    enum class ConnectReturnCode:std::uint8_t{
        Accepted  = 0;
        RefusedBadProtocol = 1;
        RefusedClientRejected = 2;
        RefusedServerUnavailabe =3;
        RefusedBadCredentials = 4 ;
        RefusedNotAuthorized = 5;
    };

    struct ConnackPacket{
        bool session_parent ;
        ConnectReturnCode return_code;
    };

    //publish

    struct PublishPacket{
        std::string topic ;
        std::vector <std::uint8_t> payload;
        std::uint8_t qos;
        bool retain;
        bool dup;

        std::optional<std::uint16_t> packet_id ;  // only present for Qos1


    }

    //puback

    struct PubackPacket{
        std::uint16_t packet_id ;
    };

    //subscribe 

    struct Subscription {
        std::string topic_filter;
        std::uint8_t  qos;
    };
    
    struct SubscribePacket{
        std::string topic_filter;
        std::uint8_t qos;
    };

    struct SubscribePacket{
        std::uint16_t packet_id;
        std::vector<Subscription>Subscription;  
    };

    //suback

    struct SubackPacket{
        std::uint16_t packet_id;
        std::vector<std::uint8_t> return_codes;
    };


// ── Parse functions ────────────────────────────────────────────────────────
// All take the raw payload bytes (everything AFTER the fixed header)
std::optional<ConnectPacket>   ParseConnect  (const std::vector<std::uint8_t>& payload);
std::optional<PublishPacket>   ParsePublish  (const FixedHeader& header,
                                              const std::vector<std::uint8_t>& payload);
std::optional<PubackPacket>    ParsePuback   (const std::vector<std::uint8_t>& payload);
std::optional<SubscribePacket> ParseSubscribe(const std::vector<std::uint8_t>& payload);

// ── Serialize functions ────────────────────────────────────────────────────
std::vector<std::uint8_t> SerializeConnack  (const ConnackPacket& packet);
std::vector<std::uint8_t> SerializePublish  (const PublishPacket& packet);
std::vector<std::uint8_t> SerializePuback   (const PubackPacket& packet);
std::vector<std::uint8_t> SerializeSuback   (const SubackPacket& packet);
std::vector<std::uint8_t> SerializePingresp ();



}


#endif