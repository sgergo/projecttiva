#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint8_t flag_t;
typedef uint8_t level_t;
typedef uint8_t packetsize_t;

typedef uint32_t defuint_t;
typedef int32_t defint_t;

typedef uint32_t tasklist_t;
typedef uint8_t logtype_t;

typedef struct {
	uint8_t error:1;
	uint8_t message:1;
	uint8_t lofasz_esti_fenyben:1;
	uint8_t dummy:5;
}logswitch_t;

typedef union {
	uint8_t byte;
	logtype_t type;
	logswitch_t bits;
}loglevel_t;

#endif