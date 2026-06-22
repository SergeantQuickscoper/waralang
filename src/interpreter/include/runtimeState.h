#ifndef WARARUNTIME
#define WARARUNTIME

/*
    The waralang runtime state stores the structs necessary
    to keep track of all mutable data and events that occur
    during the execution of a waralang program.
*/

#include <waralibs.h>
#include <stdint.h>

/*
    get mapCell with coordinated (X,Y).
    mainRS is a pointer to the runtimeState struct.
*/
#define getMapCell(mainRS, X, Y) mainRS->map->mapMatrix + (Y) * mainRS->map->width + (X)

// forward declarations
typedef struct mapCell mapCell;

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
   Struct to store spawned instances of agents on the map.
*/
typedef struct {
    Agent* instOf;
    mapCell* currLoc;
    enum direction currDir;
    size_t programCounter;
    Trie* actualParams;
} agentInst;

/*
   Array of active agent instances ordered by priority in the context
   of synchronization. Program will terminate when size reaches zero.
*/
typedef struct {
    agentInst** base;
    size_t size;
    size_t capacity;
} agentTable;


/*
    Structs coming over from our encoderSub.h but
    modified for agent considerations.
*/
struct mapCell {
    char symbol;
    size_t bid;
    agentInst* activeAgent;
};

typedef struct {
    size_t width;
    size_t height;
    mapCell* mapMatrix;
} mapData;

/*
    The following structs are used to represent logical building objects.
*/
typedef enum {
    FUNCTYPE,
    REGTYPE,
    MEMTYPE
} buildingType;

typedef struct {
    buildingType type; // always set to FUNCTYPE
    uint8_t* opcodeSeq;
    size_t opCodeSeqLength;
} func;

typedef struct {
    buildingType type; // always set to MEMTYPE
    uint8_t* base;
    size_t memSize;
} mem;

typedef struct {
    buildingType type; // always set to REGTYPE
    uint8_t* base;
    size_t filled;
} reg;


union buildingPtr {
    func* funcPtr;
    mem* memPtr;
    reg* regPtr;
};

/*
   Container for the runtime state intended to be passed between modules.
   Holds all mutable state data for the running program.
   Ideally we should only expose this struct and use it outside this file.
*/
typedef struct {
    mapData* map;
    agentTable* aliveAgentsTable;
    mapCell* spawnCell;
    enum direction spawnDirection;
    size_t baseAddressBits;
    size_t subAddressBits;
    size_t maxNonReservedBid;
    hashMap* addressToStoreLocMap;
    hashMap* buildingsTable;
} runtimeState;

#endif
