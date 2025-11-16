#ifndef BROKERINO_INCLUDE_MQTT_H_
#define  BROKERINO_INCLUDE_MQTT_H_
using namespace std ;
#include<cstdint>
#include <stdint.h>


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
    unsigned char byte;
    struct {
        unsigned retain : 1;
        unsigned qos : 2;
        unsigned dup : 1;
        unsigned type : 4;
    } bits;
};


struct Mqtt_Connect {
    union  Mqtt_Header header ;
    union {
        unsigned char byte ;
        struct {
            
        }
    }

}



}



#endif 
