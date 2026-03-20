#include <uDynamInt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uDynamInt* createUDynamInt(uint8_t byteSize){
    uDynamInt* obj = malloc(sizeof(uDynamInt));
    if(obj == NULL) return NULL;
    obj->size = byteSize;
    obj->base = malloc(obj->size * sizeof(uint8_t));
    if(obj->base == NULL) return NULL;
    memset(obj->base, 0, sizeof(uint8_t) * obj->size);
    return obj;
}

uDynamInt* incrementSizeUDynamInt(uDynamInt* obj){
    if(obj->size >= 255){
        fprintf(stderr, "DynamicIntOverflow Error! Cannot resize.\n");
        return NULL;
    }
    uDynamInt* temp = createUDynamInt(obj->size + 1);
    if(temp == NULL){
        return NULL;
    }
    // we will be using little endian btw.
    memcpy(temp->base, obj->base, sizeof(uint8_t) * obj->size);
    free(obj->base);
    free(obj);
    return temp;
}

uDynamInt* incrementValUDynamInt(uDynamInt* obj){
    if(obj == NULL){
        fprintf(stderr, "Error! Cannot increment a null uDynamInt.");
        return NULL;
    }
    uint8_t sizeRef = obj->size;
    for(uint8_t i = 0; i < sizeRef; i++){
        if(*(obj->base + i) >= 255){
            memset(obj->base + i, 0, sizeof(uint8_t));
            // control shouldn't escape this loop.
            if(i == sizeRef - 1){
                uDynamInt* temp = incrementSizeUDynamInt(obj);
                if(temp == NULL) return NULL;
                memset(temp->base + sizeRef, 1, sizeof(uint8_t));
                return temp;
            }
        }
        else{
            memset(obj->base + i, *(obj->base + i) + 1, sizeof(uint8_t));
            return obj;
        }
    }

    fprintf(stderr, "Buggy ahh code");
    return NULL;
}

uDynamInt* killUDynamicInt(uDynamInt* obj){
    free(obj->base);
    free(obj);
    return NULL;
}