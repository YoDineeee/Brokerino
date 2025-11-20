#ifndef BROKERINO_INCLUDE_MQTT_H_
#define  BROKERINO_INCLUDE_MQTT_H_
#include<cstdint>
#include <stdint.h>


#define  CONNACK_BYTE  0x20
#define PUBLISH_BYTE  0x30
#define PUBACK_BYTE   0x40
#define PUBREC_BYTE   0x50
#define PUBREL_BYTE   0x60
#define  PUBCOMP_BYTE  0x70
#define SUBACK_BYTE   0x90
#define  UNSUBACK_BYTE 0xB0
#define PINGRESP_BYTE 0xD0




// ---------------------------------------------------------------------------
// MQTT Control Packet Types (per MQTT v3.1.1 / v5.0 spec)
// --------------------------------------------------------------------------


// MQTT  Packet structure --> [fixed header ] [variable header ] [Payload]



namespace Mqtt {
    enum class ControlPacketTypes:uint8_t{
        kReserved0 = 0,
        //client only 
        kConnect = 1,
        //server only 
        kConnack = 2, 
        //client +server
        kPublish = 3,
        //client
        kPuback = 4,
        //server 
        kPubrec = 5,
        //server
        kPubrel = 6,
        //server 
        kPubcomp = 7,
        //client
        kSubscribe = 8,
        //server 
        kSuback = 9,
        //client
        kUnsubscribe = 10,
        //server 
        kUnsuback = 11,
        //client 
        kPingreq = 12,
        //server 
        kPingresp = 13,
        //client
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
            
        } bits ;
    }
    struct {

    }Payload;

}


struct Mqtt_Connack{
     union  Mqtt_Header header ;
     union {

     }
};


struct Mqtt_Publish{
     union  Mqtt_Header header ;
     struct {

     } 

    }


struct Mqtt_Puback{
     union  Mqtt_Header header ;


};


struct Mqtt_Puberc{
     union  Mqtt_Header header ;

};


struct Mqtt_Pubrel{
     union  Mqtt_Header header ;
};



struct Mqtt_Pubcomp{
     union  Mqtt_Header header ;

};

struct Mqtt_Subscribe{
     union  Mqtt_Header header ;

     struct {

     }*truples;
};

struct Mqtt_Suback{
     union  Mqtt_Header header ;
};

struct Mqtt_Suback{
     union  Mqtt_Header header ;
};


struct Mqtt_Unsubscribe{
    struct {

    }*truples ;
}

struct Mqtt_Unsuback{
     union  Mqtt_Header header ;
};


struct Mqtt_Pingreq{
     union  Mqtt_Header header ;
};



struct Mqtt_Pingresp{
    union  Mqtt_Header header ;

};

    



struct Mqtt_Disconnect{
    union  Mqtt_Header header ;
};




struct Mqtt_Auth{
    union  Mqtt_Header header ;
}; 













}



#endif 
