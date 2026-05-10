#ifndef WARARUNTIME
#define WARARUNTIME
/*
    The waralang runtime state stores the structs necessary
    to keep track of all mutable data and events that occur
    during the execution of a waralang program.
*/
#include <uDynamInt.h>

enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct {
    char* agentDefinitionID;
} agent;

typedef struct {
    agent* instOf;
    mapCell* currLoc;
    enum direction currDir;
    uDynamInt* programCounter;
    char** actualParams;
} agentInst;

typedef struct {
    agentInst* base;
    uDynamInt* size;
} agentArray;

typedef struct {
    char symbol;
    uDynamInt* bid;
    agentInst* activeAgent;
} mapCell;

typedef struct {
    uDynamInt* width;
    uDynamInt* height;
    mapCell* mapMatrix;
} mapData;


enum buildingType {
    FUNCTYPE,
    REGTYPE,
    MEMTYPE
};

typedef struct {
    uint8_t* opcodeSeq;
    uDynamInt* opCodeSeqLength;
} func;

typedef struct {
    uint8_t* base;
    uDynamInt* memSize;
} mem;

typedef struct {
    uint8_t* base;
    uDynamInt* filled;
} reg;

union buildingPtr {
    func* funcPtr;
    mem* memPtr;
    reg* regPtr;
};

typedef struct {
    uDynamInt* bid;
    enum buildingType type;
    union buildingPtr building;
} bidMap;

#endif