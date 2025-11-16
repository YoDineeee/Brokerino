#ifndef BROKERINO_INCLUDE_MQTT_H_
#define  BROKERINO_INCLUDE_MQTT_H_

#include<cstdint>

// ---------------------------------------------------------------------------
// MQTT Control Packet Types (per MQTT v3.1.1 / v5.0 spec)
// --------------------------------------------------------------------------


// MQTT  Packet structure --> [fixed header ] [variable header ] [Payload]



namespace Mqtt {
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
        kAuth = 15

    };

    enum class Qos : unsigned int  {
        kAtMostOnce =1,
        kAtLeastOnce =2 ,
        kExactlyOnce =3 
    };

   union Mqtt_Header {
    std::uint8_t byte ;
    struct {
        std::uint8_t retain =1;
        std::unit8_t qos =2 ;
        std::uint8_t dup = 1;
        std:: unit8_t type =4


    } bits;
   };




}



#endif 
