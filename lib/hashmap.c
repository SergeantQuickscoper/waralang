#include <hashmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


hashMap* createHashMap(size_t maxSize){
    if(maxSize == 0){
        fprintf(stderr, "hashmap error: maxSize cannot be 0\n");
        return NULL;
    }
    hashMap* map = malloc(sizeof(hashMap));
    if(map == NULL){
        return NULL;
    }
    map->entry = malloc(sizeof(mapEntry) * maxSize);
    if(map->entry == NULL){
        free(map);
        return NULL;
    }
    map->maxSize = maxSize;
    for(size_t i = 0; i < maxSize; i++){
        map->entry[i].keyPtr = NULL;
        map->entry[i].keySize = 0;
        map->entry[i].val = NULL;
    }
    return map;
}

// simple djb2 hash
size_t hashFunction(void* key, size_t size){
    uint8_t* bytes = (uint8_t*)key;
    size_t hash = 5381;
    for(size_t i = 0; i < size; i++){
        hash = ((hash << 5) + hash) + bytes[i];
    }
    return hash;
}


void* findKey(hashMap* map, void* key, size_t size){
    if(map == NULL){
        fprintf(stderr, "hashmap error: can't find in a NULL map\n");
        return NULL;
    }
    size_t ind = hashFunction(key, size) % map->maxSize;
    for(size_t i = 0; i < map->maxSize; i++){
        size_t probe = (ind + i) % map->maxSize;
        mapEntry* entry = map->entry + probe;
        if(entry->keyPtr == NULL){
            return NULL;
        }
        if(entry->keySize == size && memcmp(entry->keyPtr, key, size) == 0){
            return entry->val;
        }
    }
    return NULL;
}


uint8_t insertKey(hashMap* map, void* key, size_t size, void* val){
    if(map == NULL){
        fprintf(stderr, "hashmap error: can't insert on a NULL map\n");
        return 0;
    }
    size_t ind = hashFunction(key, size) % map->maxSize;
    for(size_t i = 0; i < map->maxSize; i++){
        size_t probe = (ind + i) % map->maxSize;
        mapEntry* entry = map->entry + probe;
        if(entry->keyPtr == NULL){
            entry->keyPtr = key;
            entry->keySize = size;
            entry->val = val;
            return 1;
        }
        if(entry->keySize == size && memcmp(entry->keyPtr, key, size) == 0){
            entry->val = val;
            return 1;
        }
    }
    fprintf(stderr, "hashmap error: map is full\n");
    return 0;
}

uint8_t deleteKey(hashMap* map, void* key, size_t size){
    if(map == NULL){
        fprintf(stderr, "hashmap error: can't delete on a NULL map\n");
        return 0;
    }

    size_t ind = hashFunction(key, size) % map->maxSize;
    for(size_t i = 0; i < map->maxSize; i++){
        size_t probe = (ind + i) % map->maxSize;
        mapEntry* entry = map->entry + probe;
        if(entry->keyPtr == NULL){
            return 0;
        }
        if(entry->keySize == size && memcmp(entry->keyPtr, key, size) == 0){
            entry->keyPtr = NULL;
            entry->keySize = 0;
            entry->val = NULL;

            size_t next = (probe + 1) % map->maxSize;
            while(map->entry[next].keyPtr != NULL){
                map->entry[probe] = map->entry[next];
                map->entry[next].keyPtr = NULL;
                map->entry[next].keySize = 0;
                map->entry[next].val = NULL;
                probe = next;
                next = (probe + 1) % map->maxSize;
            }
            return 1;
        }
    }
    return 0;
}

void killHashMap(hashMap* map){
    if(map == NULL){
        fprintf(stderr, "hashmap error: can't free a NULL map\n");
        return;
    }
    free(map->entry);
    free(map);
}
