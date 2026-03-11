// client_id, subscriptions ,clean session, QoS state, last will

#ifndef BROKERINO_SRC_FEATURES_PRIORITY_QUEUE_HPP_
#define BROKERINO_SRC_FEATURES_PRIORITY_QUEUE_HPP_

#include "core/mqtt_packet.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace brokerino::core{
    struct session{
        std::string client_id;
        bool clean_session;
        std::uint16_t keep_alive ; // seconds , 0 =disabled
        //topics this client  is subscribed to + their qos 
        std::unordered_map<std::string , std::uint8_t> subscriptions;
        //Unacknowledge Qos1 outgoing 
        std::unordered_map<std::uint16_t,PublishPacket> inflight;
        //PacketID counter
        std::uint16_t next_packet_id=1;
    };

    //Create a fresh session from a connect packet 
    session CreateSession (const  ConnectPacket& connect);

    //Add or update a subscription
    void AddSubscription(Session& session , const std::string& topic_filter, std::uint8_t qos);

    //store an outgoing Qos 1 packet waiting for PUBACK 

    void TrackInFlight(Session& session , const PublishPacket& packet );

    void AcknowledgeInFlight(session& session , std::uint16_t packet_id);

    std::uint16_t NextPacketId(Session& session );
}

#endif
