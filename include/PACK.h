#ifndef BROKERINO_INCLUDEPACK_H
#define BROKERINO_INCLUDE_PACK_H
#include <cstdio>
#include<cstdint>

/* we gonna read via the const uint8_t**  pointer 

*/

// bytes -> unit8_t 
uint8_t unpack_8 (const uint8_t** );

uint16_t unpack_16 ( const uint8_t**);

uint32_t unpack_32 ( const uint8_t**);

// we need to read the size of len bytes 

uint8_t *unpack_bytes ( const uint8_t **, size_t , uint8_t*);

uint16_t *unpack_bytes_16 ( const uint8_t**,size_t,uint16_t*);



void pack_8 ( uint8_t**,uint8_t);
 
void pack_16 ( uint8_t**,uint16_t );

void pack_32 (uint32_t**, uint32_t );

void pack_bytes ( uint8_t** , const uint8_t , size_t );















#endif 