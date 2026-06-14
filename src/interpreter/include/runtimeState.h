#ifndef WARARUNTIME
#define WARARUNTIME

/*
    The waralang runtime state stores the structs necessary
    to keep track of all mutable data and events that occur
    during the execution of a waralang program.
*/

#include <waralibs.h>

enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// for each agent defined in the .wl file
// immutable!
typedef struct {
    char* agentID;
    char* rawInstructions;
    char** params;
    size_t paramsLength;
} Agent;

/*
   Struct for spawned instances of agents on the map.
*/
typedef struct {
    Agent* instOf;
    mapCell* currLoc;
    enum direction currDir;
    uDynamInt* programCounter;
    Trie* actualParams;
} agentInst;

/*
   Array of active agent instances ordered by priority in the context
   of synchronization. Program will terminate when size reaches zero.
*/
typedef struct {
    agentInst* base;
    uDynamInt* size;
} agentTable;


/*
    Structs coming over from our encoderSub.h but
    modified for agent considerations.
*/

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

/*
    The following structs are used to represent logical building objecs.
*/

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

typedef struct {
    bidMap* bidMaps;
    uDynamInt* buildingCount;
} bidMapTable;


/*
   Container for the runtime state intended to be passed between modules.
   Holds all mutable state data for the running program.
   Ideally we should only expose this struct and use it outside this file.
*/

typedef struct {
    mapData* map;
    agentTable* aliveAgentsTable;
    bidMapTable* buildingsTable;
} runtimeState;

#endif
