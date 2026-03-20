#ifndef ENCODER_SUB
#define ENCODER_SUB
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <uDynamInt.h>

typedef struct {
    char symbol;
    uDynamInt* bid;
} mapCell;

typedef struct {
    void* width;
    void* height;
    void*** mapMatrix;
} mapData;

uint8_t calculateBidByteSize();

#endif
