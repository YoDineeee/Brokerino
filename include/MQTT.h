#ifndef BROKERINO_INCLUDE_MQTT_H_
#define  BROKERINO_INCLUDE_MQTT_H_

#include<cstdint>

// ---------------------------------------------------------------------------
// MQTT Control Packet Types (per MQTT v3.1.1 / v5.0 spec)
// --------------------------------------------------------------------------


namespace mqtt{
    enum class ControlPacketTypes:uint8_t{
        kReserved0 = 0,
        kConnect = 1,
        kConnack = 2, 
        kPublish = 3,
        kPuback = 4,
        kPubrec = 5,
        kPubrel = 6,
        kPubcomp = 7,
        kSubscribe = 8,
        kSuback = 9,
        kUnsubscribe = 10,
        kUnsuback = 11,
        kPingreq = 12,
        kPingresp = 13,
        kDisconnect = 14,
        kReserved15 = 15

    };

    enum class Qos {

    };


}



#endif 
