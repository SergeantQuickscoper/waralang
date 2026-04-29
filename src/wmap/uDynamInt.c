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

uint8_t isEqual(uDynamInt* val1, uDynamInt* val2){
    if(val1->size != val2->size){
        return 0;
    }
    for(uint8_t i = 0; i < val1->size; i++){
        if(*(val1->base + i) != *(val2->base + i)){
            return 0;
        }
    }
    return 1;
}

void printNum(uDynamInt* obj){
    if(obj == NULL){
        fprintf(stderr, "uDynamInt print error! Null object received.");
        return;
    }
    uDynamInt* temp = createUDynamInt(obj->size);
    memcpy(temp->base, obj->base, sizeof(uint8_t) * obj->size);
    // max should be around 617 ish (TODO: take a log10 based on size)
    // and divide into brackets to make this more efficient.
    int8_t digits[618];
    int16_t currDigits = 0;

    while(1){
        uint16_t remComb = 0;
        uint8_t allZeros = 1;
        for(uint8_t i = 0; i < obj->size; i++){
            if(*(temp->base + i) != 0){
                allZeros = 0;
                break;
            }
        }
        if(allZeros == 1) break;

        // one long div cycle
        for(int16_t i = obj->size - 1; i >= 0; i--){
            uint16_t currToDiv = remComb * 256 + *(temp->base + i);
            uint8_t rem = currToDiv % 10;
            *(temp->base + i) = currToDiv/10;
            remComb = rem;
        }

        digits[currDigits++] = remComb;
    }
    for(int16_t i = currDigits - 1; i >= 0; i--){
        fprintf(stderr, "%d", digits[i]);
    }
    free(temp->base);
    free(temp);
}

// conversion for allocation purposes :(
size_t* uDynamIntToSizeT(uDynamInt* obj){
    if(obj == NULL) return NULL;
    if(obj->size > sizeof(size_t)) return NULL; // gonna overflow
    size_t* out = (size_t*)malloc(sizeof(size_t));
    for(int i = obj->size - 1; i >= 0; i--){
        *out = (*out << 8) | obj->base[i];
    }
    return out;
}

//TODO: incharge of uDynamInt should implement this.
//I'm putting a placeholder function for now
uDynamInt* sizeTToUDynamInt(size_t num){
    return createUDynamInt(sizeof(uint8_t));
}