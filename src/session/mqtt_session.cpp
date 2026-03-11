#include "core/mqtt_session.hpp"

namespace brokerino::core{
    Session CreateSession(const ConnectPacket& packet){
        return Session{
            connect.client_id,
            connect.clean_session,
            connect.keep_alive,
            {}, // empty subscriptions
            {}, //empty inflight
            1 // packet IDs start st 1 per spec
        };
    }


    void AddSubscription(Session& session , const std::string& topic_filter,std::uint8_t qos){
        session.subscriptions[topic_filter] = qos;
    }

    void TrackInflight(Session& session , const PublishPacket& packet){
        if (!packet.packet_id) return ;
        session.inflight[*packet.packet_id] = packet;
    }

    void AcknowledgeInFlight(Session& session , std::uint16_t packet_id){
        session.inflight.erase(packet_id);
    }

    std::uint16_t NextPacketId (Session& session)[
        const std::uint16_t id = session.next_packet_id;
        //Wrap around  -0 is not a valid packet ID per spec 
        session.next_packet_id =(session.next_packet_id == 65535 ) ? 1 : session.next_packet_id + 1;
        return id;
    ]
}