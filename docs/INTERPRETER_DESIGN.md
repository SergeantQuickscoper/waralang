# `waralang` Interpreter Design

This document outlines the requirements and architecture of the `waralang`
interpreter.

## Basic modules
The interpreter can be thought of as a combination of the following modules:

- `interpreter-core` - reads the source code and sends the corresponding
commands to modify the `runtime-state`.

- `runtime-state` - a module that keeps track of the current state of the runtime.

- `decoder` - a module that reads a `.wmap` file and sets up the initial
`runtime-state`.

- `view-out` - a module that reads the current runtime state and whose sole
job is to show it to the user in a user friendly way. Maybe we could also use
it to collect user input.

Eveything else can be thought of a sub-module of either of these modules.

## Basic flow of execution

When a source program is executed by the interpreter a rough outline of
execution would be:

1. The source program is read into memory.

2. The first section tells us what the `wmap` file is and thus the `decoder`
has the job of reading through it and setting up the initial `runtime-state`.

3. The initial `runtime-state` contains a table of accessible memory blocks,
building functionalities, source code grid, current tick rate value, active
agents table, etc.

4. All elements of the `runtime-state` are accessible by the `interpreter-core`
and the job of the `interpreter-core` is to calculate everything that happens
during the course of a tick, and apply the updates back to the `runtime-state`.
Basic OPCODE definiitions and so on are all contained within `interpreter-core`.

5. Finally once a tick is written and the `runtime-state` updated, the
`interpreter-core` calls the `view-out` function to refresh the frame
presented to the user. `view-out` will handle both the `main-agent` view
and the `map-view`.

6. A `tick` is done and the `interpreter-core` proceeds to calculate the next
`tick`. Any illegal operations would be recognized when the `interpreter-core`
tries to perform it on the `runtime-state`, the core would recieve the error
and then raise an exception to the user.

This design is inspired by and intentionally mirrors a CPU (`intepreter-core`)
and Primary Memory (`runtime-state`) with the former reading and writing to
the latter every CPU cycle (`tick`).

The rest of this document will go into more detail of each module.

## Runtime State

The runtime state keeps track of state of every entity on the map that
is mutable. This mostly just includes mapData (yes mapData can change
during runtime!), agents and memory. The simplest solution to represent
the map data would be to reuse the structs previously used to hold
information for the encoder, namely `mapCell` and `mapData` structs.
These structs would act as a representation for all spacial data on the
map, and each `mapCell` would contain a `bid` to represent its functionality.
Of course this implies that we would need a table to map `bids` to their
corresponding logical objects. This gets a little weird because not all `bids`
do correspond to a logical object like a building. Reserved `bids` for example
can have all their information inferred from the number itself. I think
it is important to define what can and cannot change during runtime so:

Things that could change (and hence included in runtime state):

- `mapData` - Though we will be reusing the `mapData` struct from `wcoder`,
we will be adding extra fields like `currAgent` to point to the current agent
on that cell to make checking for collisions easier. No two agents can be on
the same cell. Furthermore it would be interesting if we add buildings that
could shift around map functionality later so for existing and future expansion
reasons `mapData` is considered mutable.

The struct for `mapCell` and `mapData` will look similar to this:

```
typedef struct {
    char symbol;
    uDynamInt* bid;
    agent* activeAgent;
} mapCell;

typedef struct {
    uDynamInt* width;
    uDynamInt* height;
    mapCell* mapMatrix;
} mapData;

```
- `agents` array - The most important data structure from the perspective of
the `interpreter-core`. Contains information about all active agents. The
program terminates whenever the array is empty at the start of a tick.

```
enum direction{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct {
    // TODO: add life cycle address
    agent* instOf;
    mapCell* currLoc;
    enum direction currDir;
    uDynamInt* programCounter; // inst to be executed next
    char** actualParams; // length of this should match instOf->paramLength
} agentInst;

// higher priority for agents earlier in the array
typedef struct {
    agentInst* base;
    uDynamInt* size; // cuz why NOT
} agentsTable;

```
- `buildings` - We would need to store a bid to `building` map, and then
the individual building structs themselves.

```
enum buildingType{
    FUNCTYPE,
    REGTYPE,
    MEMTYPE
};

typedef struct{
    uint8_t* opcodeSeq;
    uDynamInt* opCodeSeqLength;
} func;

typedef struct{
    uint8_t* base;
    uDynamInt* memSize;
} mem;

typedef struct{
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


```

Finally for ease of just passing in one parent object to whatever functions
modify the runtime state let the following struct as a container for the
current runtime:

```

typedef struct {
    mapData* map;
    agentTable* aliveAgentsTable;
    bidMapTable* buildingsTable;
} runtimeState;


```


## Interpreter core
This module has alot of varied responsbilities including storing stuff
that will not change during runtime, calculating tick outcomes and updating
the runtime state and also reading the source code and calculating substituitions
and function calls.

### Non-volatile data structs
These are structs that do no change during runtime and are initialized once when
the decoder parses the `wmap` file. The exact files these will be a part of are yet
to be determined but they will probably be logically cohesive, for example the
`addressDecode` structs will be located in `coreAddDec.h` along with the helper
functions associated with decoding addresses;

```
typedef struct{
    uDynamicInt* baseBits;
    uDynamicInt* subAddBits;
    uDyanmicInt* wordSize; // compute and store
} runtimeConstants;

enum memoryType{
    MEMTYPE,
    REGTYPE
}

union storageBuilding{
    mem* memoryBuilding,
    reg* registerBuilding
};

enum storageBuildingType{
    REGTYPE,
    MEMTYPE
};

typedef struct{
    enum storageBuildingType type;
    union storageBuilding data;
} storageBuildingFlagged;

// Common trie from `trie.h` to store and search REG and MEM baseAddresses.
// TrieNode.data will point to storageBuildingFlagged.
Trie* storageBuildingTrie;

typedef struct {
    char* agentID;
    uint8_t nameLength;
    char* rawInstructions;
    char** params;
    uint8_t paramLength;
} agent;
```

### Opcode Implementations

We would also need callable implementations of all of the available opcodes
in something like `coreOps.h`. It would probably be a good idea to let the
buildings take in the structs of `runtime state` as parameters. For example:

```
// The user supplied addresses would be decoded.
// It is not the job of loadOP to decode addresses.
void loadOP(REG* dest, MEM* memoryObj, uDynamicInt* startInd, uDynamicInt* byteCount);

```

### `wmap` Decoder

A module probably named `wmapDecoder.h` will be used to contain decodes
logic for `wmap`.