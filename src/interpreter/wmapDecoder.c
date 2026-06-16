#include "wmapDecoder.h"

runtimeState* decodeWmap(char* wmapPath){

    // temp variables to help with decoding alloc
    uint8_t heightBytes;
    uint8_t widthBytes;
    uint8_t bidSizeBytes;
    uint8_t spawnXBytes;
    uint8_t spawnYBytes;
    uint8_t wordSizeBytes;
    char spawnDirectionTemp;

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

    fread(&(state->baseAddressBits), sizeof(uint8_t), wordSizeBytes, wmapFile);
    fread(&(state->subAddressBits), sizeof(uint8_t), wordSizeBytes, wmapFile);

    size_t heightS = state->map->height;
    size_t widthS = state->map->height;
    size_t maxBidExcludingReserved = 0;
    for(size_t i = 0; i < heightS; i++){
        for(size_t j = 0; j < widthS; j++){
           mapCell* curr = (state->map->mapMatrix + i * widthS + j);
           fread(&(curr->symbol), sizeof(uint8_t), 1, wmapFile);
           fread(&(curr->bid), sizeof(uint8_t), sizeof(bidSizeBytes),
           wmapFile);
           if(curr->bid > (1 << bidSizeBytes)){
            fprintf(stderr, "wmapDecoder error: unexpected bid %zu encountered"
                "within cells.\nValue is greater than defined bidSize of %zu"
                "bytes", curr->bid, bidSizeBytes);
           }
           if(curr->bid < (1 << bidSizeBytes) - 3 && curr->bid >
           maxBidExcludingReserved){
            maxBidExcludingReserved = curr->bid;
           }
        }
    }

    state->spawnCell = getMapCell(state, uDynamIntToSizeT(spawnX), uDynamIntToSizeT(spawnY));

    fclose(wmapFile);
    return state;
}
