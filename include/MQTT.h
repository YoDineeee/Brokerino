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
        //Connection request
        kConnect = 1,
        //Connect acknowledgment
        kConnack = 2, 
        //Publish message
        kPublish = 3,
        //Publish acknowledgment (QoS 1)
        kPuback = 4,
        //Publish received (QoS 2 delivery part 1)
        kPubrec = 5,
        //Publish release (QoS 2 delivery part 2)
        kPubrel = 6,
        //Publish complete (QoS 2 delivery part 3)
        kPubcomp = 7,
        //Subscribe request
        kSubscribe = 8,
        //Subscribe acknowledgment
        kSuback = 9,
        //Unsubscribe request
        kUnsubscribe = 10,
        // 	Unsubscribe acknowledgment
        kUnsuback = 11,
        //PING request
        kPingreq = 12,
        //PING response
        kPingresp = 13,
        //Disconnect notification
        kDisconnect = 14,
        
        kReserved15 = 15

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
            int reserved :1;
            unsigned clean_session:1;
            unsigned will :1;
            unsigned will_qos:2;
            unsigned will_retain:1;
            unsigned password :1;
            unsigned username :1;      
        } bits ;
    } ;

    struct{
        unsigned short kepaalive ;
        unsigned char *client_id;
        unsigned char *user_name;
        unsigned char *password;
        unsigned char *will_message;
        unsigned char *will_topic;
    }Payload;
};



struct Mqtt_Connack{
     union  Mqtt_Header header ;
     union {
        unsigned char byte ;
        struct{
            unsigned session_presnet :1;
            unsigned reserved :7;

        } bits;

     };
     unsigned char return_code ;
};




struct Mqtt_Subscribe{
     union  Mqtt_Header header ;
     unsigned short pkt_id;
     unsigned short tupels_len ;
     struct {
        unsigned short topic_len;
        unsigned char *topic;
        unsigned qos ;
     }*truples;
};


struct Mqtt_Unsubcribe {
     union  Mqtt_Header header ;
     unsigned short pkt_id;
     unsigned short tupels_len ;
     struct {
        unsigned short topic_len;
        unsigned char *topic;
        unsigned qos ;
     }*truples;
};



struct Mqtt_Suback{
    union  Mqtt_Header header ;
    unsigned short pkt_id;
    unsigned short return_codes_len ;
    unsigned char *return_codes ;  
};

struct Mqtt_Publish{
   union  Mqtt_Header header ;
   unsigned short pkt_id;
   unsigned short topiclen;
   unsigned short *topic;
   unsigned char payloadlen;
   unsigned char *payload;
};





struct Mqtt_Ack{
     union  Mqtt_Header header ;
     unsigned short pkt_id;
};

// since PUBACK, PUBREC, PUBREL, PUBCOMP, UNSUBACK have the same packet structure 

typedef struct Mqtt_Ack Mqtt_Puback ;
typedef struct Mqtt_Ack Mqtt_Puberc;
typedef struct Mqtt_Ack Mqtt_Pubrel;
typedef struct Mqtt_Ack Mqtt_Pubcomp;
typedef struct Mqtt_Ack Mqtt_Unsuback;

// since DISCONNECT ,PINGREQ and PINGRESP have the same fixed-header  structure
typedef union Mqtt_Header Mqtt_Disconnect;
typedef union Mqtt_Header Mqtt_Pingreq;
typedef union Mqtt_Header Mqtt_Pingresp;


//generic Mqtt packet 

union Mqtt_Packet{
    struct Mqtt_Ack ack;
    union Mqtt_Header header ;
    struct Mqtt_Connect connect ;
    struct Mqtt_Connack connack ;
    struct Mqtt_Subscribe subscribe ;
    struct Mqtt_Suback suback ;
    struct Mqtt_Unsubcribe unsubscribe ;
    struct Mqtt_Publish publish ;
    
};


// int Mqtt_encode_length(unsigned char * , size_t);
// unsigned long long Mqtt_decode_length(const unsigned char **);
// int unpack_mqtt_packet(const unsigned char *, union Mqtt_packet *);
// unsigned char *pack_mqtt_packet(const union Mqtt_packet *, unsigned);



// union Mqtt_Header *mqtt_packet_header(unsigned char);
// struct Mqtt_Ack *mqtt_packet_ack(unsigned char , unsigned short);
// struct Mqtt_Connack *mqtt_packet_connack(unsigned char, unsigned char, unsigned char);
// struct Mqtt_Suback *mqtt_packet_suback(unsigned char, unsigned short,
//                                        unsigned char *, unsigned short);
// struct Mqtt_Publish *mqtt_packet_publish(unsigned char, unsigned short, size_t,
//                                          unsigned char *, size_t, unsigned char *);
// void mqtt_packet_release(union Mqtt_Packet *, unsigned)
























#endif //hello world    
