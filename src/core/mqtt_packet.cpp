#include "core/mqtt_packet.hpp"

namespace brokerino::core {

//  Helpers 

static std::optional<std::uint16_t> ReadUint16(const std::vector<std::uint8_t>& buf,
                                                std::size_t& pos) {
    if (pos + 2 > buf.size()) return std::nullopt;
    const std::uint16_t val = (buf[pos] << 8) | buf[pos + 1];
    pos += 2;
    return val;
}

static std::optional<std::string> ReadString(const std::vector<std::uint8_t>& buf,
                                              std::size_t& pos) {
    auto len = ReadUint16(buf, pos);
    if (!len) return std::nullopt;
    if (pos + *len > buf.size()) return std::nullopt;

    std::string str(buf.begin() + pos, buf.begin() + pos + *len);
    pos += *len;
    return str;
}

// CONNECT 

std::optional<ConnectPacket> ParseConnect(const std::vector<std::uint8_t>& payload) {
    std::size_t pos = 0;

    // Protocol name must be "MQTT"
    auto protocol_name = ReadString(payload, pos);
    if (!protocol_name || *protocol_name != "MQTT") return std::nullopt;

    // Protocol level must be 4 for MQTT 3.1.1
    if (pos >= payload.size()) return std::nullopt;
    if (payload[pos++] != 4)   return std::nullopt;

    // Connect flags
    if (pos >= payload.size()) return std::nullopt;
    const std::uint8_t flags = payload[pos++];

    const bool clean_session = (flags >> 1) & 1;
    const bool has_username  = (flags >> 7) & 1;
    const bool has_password  = (flags >> 6) & 1;

    // Keep alive
    auto keep_alive = ReadUint16(payload, pos);
    if (!keep_alive) return std::nullopt;

    // Client ID
    auto client_id = ReadString(payload, pos);
    if (!client_id) return std::nullopt;

    // Optional username / password
    std::string username;
    std::string password;

    if (has_username) {
        auto u = ReadString(payload, pos);
        if (!u) return std::nullopt;
        username = std::move(*u);
    }

    if (has_password) {
        auto p = ReadString(payload, pos);
        if (!p) return std::nullopt;
        password = std::move(*p);
    }

    return ConnectPacket{
        std::move(*client_id),
        std::move(username),
        std::move(password),
        *keep_alive,
        clean_session
    };
}

//  CONNACK 

std::vector<std::uint8_t> SerializeConnack(const ConnackPacket& packet) {
    std::vector<std::uint8_t> out = EncodeFixedHeader(PacketType::CONNACK, 0, 2);
    out.push_back(packet.session_present ? 1 : 0);
    out.push_back(static_cast<std::uint8_t>(packet.return_code));
    return out;
}

//  PUBLISH 

std::optional<PublishPacket> ParsePublish(const FixedHeader& header,
                                           const std::vector<std::uint8_t>& payload) {
    std::size_t pos = 0;

    const bool         dup    = (header.flags >> 3) & 1;
    const std::uint8_t qos    = (header.flags >> 1) & 0x03;
    const bool         retain =  header.flags        & 1;

    auto topic = ReadString(payload, pos);
    if (!topic) return std::nullopt;

    std::optional<std::uint16_t> packet_id;
    if (qos > 0) {
        packet_id = ReadUint16(payload, pos);
        if (!packet_id) return std::nullopt;
    }

    std::vector<std::uint8_t> msg_payload(payload.begin() + pos, payload.end());

    return PublishPacket{
        std::move(*topic),
        std::move(msg_payload),
        qos,
        retain,
        dup,
        packet_id
    };
}

std::vector<std::uint8_t> SerializePublish(const PublishPacket& packet) {
    std::vector<std::uint8_t> body;

    // Topic
    const std::uint16_t topic_len = static_cast<std::uint16_t>(packet.topic.size());
    body.push_back(topic_len >> 8);
    body.push_back(topic_len & 0xFF);
    for (auto c : packet.topic) body.push_back(static_cast<std::uint8_t>(c));

    // Packet ID (QoS 1 only)
    if (packet.qos > 0 && packet.packet_id) {
        body.push_back(*packet.packet_id >> 8);
        body.push_back(*packet.packet_id & 0xFF);
    }

    // Payload
    body.insert(body.end(), packet.payload.begin(), packet.payload.end());

    const std::uint8_t flags = (packet.dup    ? 0x08 : 0)
                             | (packet.qos << 1)
                             | (packet.retain ? 0x01 : 0);

    auto out = EncodeFixedHeader(PacketType::PUBLISH, flags,
                                 static_cast<std::uint32_t>(body.size()));
    out.insert(out.end(), body.begin(), body.end());
    return out;
}

// PUBACK 

std::optional<PubackPacket> ParsePuback(const std::vector<std::uint8_t>& payload) {
    std::size_t pos = 0;
    auto packet_id = ReadUint16(payload, pos);
    if (!packet_id) return std::nullopt;
    return PubackPacket{*packet_id};
}

std::vector<std::uint8_t> SerializePuback(const PubackPacket& packet) {
    auto out = EncodeFixedHeader(PacketType::PUBACK, 0, 2);
    out.push_back(packet.packet_id >> 8);
    out.push_back(packet.packet_id & 0xFF);
    return out;
}

// SUBSCRIBE 

std::optional<SubscribePacket> ParseSubscribe(const std::vector<std::uint8_t>& payload) {
    std::size_t pos = 0;

    auto packet_id = ReadUint16(payload, pos);
    if (!packet_id) return std::nullopt;

    std::vector<Subscription> subscriptions;

    while (pos < payload.size()) {
        auto topic_filter = ReadString(payload, pos);
        if (!topic_filter) return std::nullopt;
        if (pos >= payload.size()) return std::nullopt;

        const std::uint8_t qos = payload[pos++] & 0x03;
        subscriptions.push_back({std::move(*topic_filter), qos});
    }

    if (subscriptions.empty()) return std::nullopt;

    return SubscribePacket{*packet_id, std::move(subscriptions)};
}

//  SUBACK 

std::vector<std::uint8_t> SerializeSuback(const SubackPacket& packet) {
    const std::uint32_t remaining =
        2 + static_cast<std::uint32_t>(packet.return_codes.size());

    auto out = EncodeFixedHeader(PacketType::SUBACK, 0, remaining);
    out.push_back(packet.packet_id >> 8);
    out.push_back(packet.packet_id & 0xFF);
    for (auto code : packet.return_codes) out.push_back(code);
    return out;
}

// PINGRESP 

std::vector<std::uint8_t> SerializePingresp() {
    return EncodeFixedHeader(PacketType::PINGRESP, 0, 0);
}

} // namespace brokerino::core