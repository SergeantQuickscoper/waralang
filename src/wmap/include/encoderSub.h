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

/*
  ideally we should implement some kind of chunking here but
  for now it should be fine to load the entire map into memory
  while encoding, after all our upper limit of 2^4096 bytes of
  map data is not a concern rn :D
*/
mapData* loadMapData();

uint8_t calculateBidByteSize();

#endif
