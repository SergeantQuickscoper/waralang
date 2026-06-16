#include "wmapDecoder.h"

runtimeState* decodeWmap(char* wmapPath){
    // temp variables to help with decoding alloc
    uint8_t heightBytes = 0;
    uint8_t widthBytes = 0;
    uint8_t bidSizeBytes = 0;
    uint8_t spawnXBytes = 0;
    uint8_t spawnYBytes = 0;
    uint8_t wordSizeBytes = 0;
    char spawnDirectionTemp = 0;

    FILE* wmapFile = fopen(wmapPath, "rb");
    if(wmapFile == NULL){
        fprintf(stderr, "wmapDecoder error: Failed to open wmap file.\n");
        return NULL;
    }

    runtimeState* state = (runtimeState*)malloc(sizeof(runtimeState));
    if(state == NULL){
        fprintf(stderr, "wmapDecoder error: Failed to allocate memory for runtime state.\n");
        return NULL;
    }

    state->map = (mapData*)malloc(sizeof(mapData));
    if(state->map == NULL){
        fprintf(stderr, "wmapDecoder error: Failed to allocate memory for map data.\n");
        free(state);
        return NULL;
    }

    /*
        File reading actually starts here.
        TODO: replace this with helpers.
    */
    fread(&heightBytes, sizeof(uint8_t), 1, wmapFile);
    fread(&widthBytes, sizeof(uint8_t), 1, wmapFile);
    uDynamInt* height = createUDynamInt(heightBytes);
    uDynamInt* width = createUDynamInt(widthBytes);
    if(height == NULL || width == NULL){
        fprintf(stderr, "wmapDecoder error: Failed to allocate memory for height and width.\n");
        // TODO: replace this with a proper state free method
        free(height);
        free(width);
        return NULL;
    }
    fread(height->base, sizeof(uint8_t), heightBytes, wmapFile);
    fread(width->base, sizeof(uint8_t), widthBytes, wmapFile);
    state->map->height = uDynamIntToSizeT(height);
    state->map->width = uDynamIntToSizeT(width);
    fread(&bidSizeBytes, sizeof(uint8_t), 1, wmapFile);

    uDynamInt* spawnX = createUDynamInt(widthBytes);
    fread(spawnX->base, sizeof(uint8_t), widthBytes, wmapFile);
    uDynamInt* spawnY = createUDynamInt(heightBytes);
    fread(spawnY->base, sizeof(uint8_t), heightBytes, wmapFile);
    fread(&spawnDirectionTemp, sizeof(uint8_t), 1, wmapFile);

    switch(spawnDirectionTemp){
        case '>':
            state->spawnDirection = RIGHT;
            break;
        case 'v':
            state->spawnDirection = DOWN;
            break;
        case '<':
            state->spawnDirection = LEFT;
            break;
        case '^':
            state->spawnDirection = UP;
            break;
        default:
            fprintf(stderr, "wmapDecoder error: Invalid spawn direction parsed\n");
            return NULL;
    }

    fread(&wordSizeBytes, sizeof(uint8_t), 1, wmapFile);

    state->aliveAgentsTable = malloc(sizeof(agentTable));
    state->aliveAgentsTable->capacity = 1024;
    state->aliveAgentsTable->size = 0;
    state->aliveAgentsTable->base = malloc(sizeof(agentInst*) *
    state->aliveAgentsTable->capacity);

    // this is to get rid of garbage values
    // also could just memset the entire state
    // struct to zero too i suppose
    state->baseAddressBits = 0;
    state->subAddressBits = 0;
    fread(&(state->baseAddressBits), sizeof(uint8_t), wordSizeBytes, wmapFile);
    fread(&(state->subAddressBits), sizeof(uint8_t), wordSizeBytes, wmapFile);

    size_t heightS = state->map->height;
    size_t widthS = state->map->width;
    size_t maxBidExcludingReserved = 0;

    state->map->mapMatrix = (mapCell*)malloc(sizeof(mapCell) *
    state->map->width * state->map->height);

    if(state->map->mapMatrix == NULL){
        fprintf(stderr, "wmapDecoder error: unable to allocate memory for map"
            "data, your map is probably too big!\n");
        return NULL;
    }

    for(size_t i = 0; i < heightS; i++){
        for(size_t j = 0; j < widthS; j++){
           mapCell* curr = (state->map->mapMatrix + i * widthS + j);
           fread(&(curr->symbol), sizeof(char), 1, wmapFile);
           curr->bid = 0;
           fread(&(curr->bid), sizeof(uint8_t), bidSizeBytes,
           wmapFile);
           // holy redundant
           if(curr->bid > (1 << (bidSizeBytes * 8))){
            fprintf(stderr, "\nwmapDecoder error: unexpected bid %zu encountered"
                " within cells. Value is greater than defined bidSize of %zu"
                " bytes.\n", curr->bid, bidSizeBytes);
                return NULL;
           }
           if(curr->bid < (1 << (bidSizeBytes * 8)) - 3 && curr->bid >
           maxBidExcludingReserved){
            maxBidExcludingReserved = curr->bid;
           }
           fprintf(stderr, "%c", curr->symbol);
        }
        fprintf(stderr, "\n");
    }
    state->spawnCell = getMapCell(state, uDynamIntToSizeT(spawnX), uDynamIntToSizeT(spawnY));
    state->maxNonReservedBid =  maxBidExcludingReserved;

    for(size_t i = 0; i < 6; i++){
        uint8_t opCodeBytes = 0;
        size_t opCodeSeqLength = 0;
        uint8_t firstOp = 0;
        fread(&opCodeBytes, sizeof(uint8_t), 1, wmapFile);
        fread(&opCodeSeqLength, sizeof(uint8_t), opCodeBytes, wmapFile);
        if(opCodeSeqLength == 0){
            fprintf(stderr, "\nwmapDecoder error: an opcode sequence must"
            "have at least one opcode in it.");
            return NULL;
        }
        /*
            MEM OPCODE - 00000001
            REG OPCODE - 00000010
        */
       fread(&(firstOp), sizeof(uint8_t), 1, wmapFile);
       switch(firstOp){
        case 1:
            // MEM logic
            break;
        case 2:
            // REG logic
            break;
        default:
            // FUNC logic
            break;
       }
    }


    fclose(wmapFile);
    return state;
}
