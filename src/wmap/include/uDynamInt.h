#ifndef UDYNAMIC_INT
#define UDYNAMIC_INT
#include <stdint.h>

/*
    Functions to execute ops on uDynamicInt, which is a dumb name because for
    the scope of `waralang` this is just a counter variable. Unsigned and
    should support integers upto 2^2048 (256 bytes). Numbers are represented
    in little endian.
*/

// we are never "subtracting" here so these fields should be enough
typedef struct {
    uint8_t* base;
    uint8_t size;
} uDynamInt;

uDynamInt* createUDynamInt(uint8_t byteSize);

uDynamInt* incrementSizeUDynamInt(uDynamInt* obj);

uDynamInt* incrementValUDynamInt(uDynamInt* obj);

// convienience method
uDynamInt* killUDynamicInt(uDynamInt* obj);

#endif