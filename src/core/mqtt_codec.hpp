/**
 * @file mqtt_codec.hpp
 * @brief MQTT 3.1.1 fixed header encoding/decoding utilities.
 */

 



#ifndef BROKERINO_SRC_CORE_MQTT_CODEC_HPP
#define BROKERINO_SRC_CORE_MQTT_CODEC_HPP

#include <cstdint>
#include <vector>
#include <optional>

namespace brokerino::core {

// ============================================================
// MQTT Packet Types (MQTT 3.1.1 Specification)
// ============================================================

// Represents MQTT Control Packet types as defined by the protocol.
// Values correspond to the high 4 bits of the fixed header byte.
enum class PacketType : std::uint8_t {
    CONNECT     = 1,
    CONNACK     = 2,
    PUBLISH     = 3,
    PUBACK      = 4,
    SUBSCRIBE   = 8,
    SUBACK      = 9,
    PINGREQ     = 12,
    PINGRESP    = 13,
    DISCONNECT  = 14,
};

// ============================================================
// Fixed Header Structures
// ============================================================

// Represents the MQTT Fixed Header (first part of every packet).
// Contains packet type, flags, and remaining length.
struct FixedHeader {
    PacketType   type;              // Control packet type
    std::uint8_t flags;             // Flags specific to the packet type
    std::uint32_t remaining_length; // Length of variable header + payload
};

// Result of decoding a fixed header from a byte buffer.
struct DecodeResult {
    FixedHeader header;      // Parsed fixed header
    std::size_t header_size; // Number of bytes consumed (including length encoding)
};

// ============================================================
// Encoding / Decoding Utilities
// ============================================================

// Encodes the "Remaining Length" field using MQTT variable-length encoding.
// This uses 1 to 4 bytes depending on the value.
std::vector<std::uint8_t>
EncodeRemainingLength(std::uint32_t length);

// Decodes the fixed header from a buffer.
// Returns std::nullopt if the buffer is incomplete or malformed.
std::optional<DecodeResult>
DecodeFixedHeader(const std::vector<std::uint8_t>& buf);

// Encodes a full fixed header (type + flags + remaining length).
// Returns a byte vector ready to be prepended to a packet body.
std::vector<std::uint8_t>
EncodeFixedHeader(PacketType type,
                  std::uint8_t flags,
                  std::uint32_t remaining_length);

} // namespace brokerino::core

#endif // BROKERINO_SRC_CORE_MQTT_CODEC_HPP

