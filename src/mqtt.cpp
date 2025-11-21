#include <iostream>
#include <cstring>
#include "MQTT.h"

static size_t unpack_mqtt_connect(const unsigned char*,
                                  union Mqtt_Header*,
                                  union Mqtt_Packet*);

static size_t unpack_mqtt_publish(const unsigned char*,
                                  union Mqtt_Header*,
                                  union Mqtt_Packet*);

static size_t unpack_mqtt_subscribe(const unsigned char*,
                                    union Mqtt_Header*,
                                    union Mqtt_Packet*);

static size_t unpack_mqtt_unsubscribe(const unsigned char*,
                                      union Mqtt_Header*,
                                      union Mqtt_Packet*);

static size_t unpack_mqtt_ack(const unsigned char*,
                              union Mqtt_Header*,
                              union Mqtt_Packet*);

static unsigned char* pack_mqtt_header(const union Mqtt_Header*);
static unsigned char* pack_mqtt_ack(const union Mqtt_Packet*);
static unsigned char* pack_mqtt_connack(const union Mqtt_Packet*);
static unsigned char* pack_mqtt_suback(const union Mqtt_Packet*);
static unsigned char* pack_mqtt_publish(const union Mqtt_Packet*);


