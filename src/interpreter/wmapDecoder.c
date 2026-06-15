#include "wmapDecoder.h"

runtimeState* decodeWmap(char* wmapPath){

    // storage variables to help with decoding alloc
    uint8_t heightBytes;
    uint8_t widthBytes;
    uint8_t bidSizeBytes;
    uint8_t spawnXBytes;
    uint8_t spawnYBytes;
    uint8_t wordSizeBytes;

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
    // NOTE/TODO for @sergeantQuickscoper: mapmatrix must be initialized before this
    state->spawnCell = getMapCell(state, uDynamIntToSizeT(spawnX), uDynamIntToSizeT(spawnY));

    // TODO for @sergeantQuickscoper: turn this from char to enum
    fread(&(state->spawnDirection), sizeof(uint8_t), 1, wmapFile);
    fread(&wordSizeBytes, sizeof(uint8_t), 1, wmapFile);

    state->aliveAgentsTable = malloc(sizeof(agentTable));
    
    state->aliveAgentsTable->capacity = 1024;
    state->aliveAgentsTable->size = 0;
    state->aliveAgentsTable->base = malloc(sizeof(agentInst*) * state->aliveAgentsTable->capacity);

    fclose(wmapFile);
    return state;
}
