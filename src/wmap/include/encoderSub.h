#ifndef ENCODER_SUB
#define ENCODER_SUB
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <uDynamInt.h>
#include <json-c/json.h>

/*
    We will use a NULL bid pointer to represent buildings whose
    bids have not been set by the dfs procedure.
*/
typedef struct {
    char symbol;
    uDynamInt* bid;
} mapCell;

typedef struct{
    uDynamInt* bid;
    uDynamInt* opcodeCount;
    uint8_t* opcodes;
    uint8_t baseAddressBytes;
    char* baseAddress;
    uint8_t memSizeBytes;
    uint8_t* memSize;
} bidMap;


typedef struct {
    uDynamInt* width;
    uDynamInt* height;
    mapCell* mapMatrix;
    bidMap* buildings;
    uDynamInt* bidCount;
} mapData;

/*
  ideally we should implement some kind of chunking here but
  for now it should be fine to load the entire map into memory
  while encoding, after all our upper limit of 2^4096 bytes of
  map data is not a concern rn :D
*/
mapData* initializeMapData(FILE* mapTextFile, json_object* configObj);

uint8_t calculateBidByteSize();

#endif
