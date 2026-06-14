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
    state->map->height = height;
    state->map->width = width;
    fread(state->map->height->base, sizeof(uint8_t), heightBytes, wmapFile);
    fread(state->map->width->base, sizeof(uint8_t), widthBytes, wmapFile);
    fread(&bidSizeBytes, sizeof(uint8_t), 1, wmapFile);

    state->spawnX = createUDynamInt(widthBytes);
    fread(state->spawnX->base, sizeof(uint8_t), widthBytes, wmapFile);
    state->spawnY = createUDynamInt(heightBytes);
    fread(state->spawnY->base, sizeof(uint8_t), heightBytes, wmapFile);

    fread(&(state->spawnDirection), sizeof(uint8_t), 1, wmapFile);
    fread(&wordSizeBytes, sizeof(uint8_t), 1, wmapFile);


    fclose(wmapFile);
    return state;
}
