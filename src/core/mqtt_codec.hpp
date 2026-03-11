#ifndef BROKERINO_SRC_CORE_MQTT_CODEC_HPP_
#define BROKERINO_SRC_CORE_MQTT_CODEC_HPP_

#include<cstdint>
#include<vector>
#include<optional>

namespace brokerino::core {

    enum class PacketType : std::uint8_t{
        CONNECT = 1;
        CONNACK  = 2;
        PUBLISH = 3 ;
        PUBACK = 4 ;
        PUBREC = 5;
        PUBREL = 6;
        DISCONNECT = 14 ;
    }

    struct FixedHeader{
        PacketType type ;
        std::uint8_t flags;
        std::uint32_t remaining_length ;
    };

    struct DecodeResult{
        FixedHeader header ;
        std::size_t  header_size ;
    }

 
// Encode remaining length into MQTT variable-length format (1-4 bytes)
 std::vector<std::uint8_t> EncodeRemaininglength(std::uint32_t length );

std::optional<DecodeResult> DecodeFixedHeader(const std::vector<std::uint8_t>& buf);

std::vector<std::uint8_t> EncodeFixedHeader(PacketType type , std::uint8_t flags , std::uint32_t remaining_length);


}


#endif