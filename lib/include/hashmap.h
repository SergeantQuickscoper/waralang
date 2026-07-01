#ifndef WARHASHMAP
#define WARHASHMAP

/*
    Library that provides a simple hashmap filled that is intended to be used
    to map to building structs like func, mem and reg in waralang. However,
    implementation is done generically through void* so we could use it for
    our use cases of memory addresses names and bidMaps. Each key is just a
    void* pointer + a size value of the object its pointing to. The hash
    function is run on value of the objects for the size specified. Collision
    resolution is a simple linear probe. The hash function is just djb2

    TODO: dynamic hashmap :OOOO
*/

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void* keyPtr;
    size_t keySize;
    void* val;
} mapEntry;

typedef struct {
    mapEntry* entry;
    size_t maxSize;
} hashMap;

/*
    Creates an empty hashMap object.
*/
hashMap* createHashMap(size_t maxSize);

/*
    Hash function that returns insertion index of a key in the hash table.
*/
size_t hashFunction(void* key, size_t size);

/*
    Returns value of a key in the hash table. Returns NULL if key is not
    present.
*/
void* findKey(hashMap* map, void* key, size_t size);

/*
    Inserts a key-value pair into the hash table.
*/
uint8_t insertKey(hashMap* map, void* key, size_t size, void* val);

/*
    Deletes an entry from the hash table. Returns 1 if succesful,
    else 0.
*/
uint8_t deleteKey(hashMap* map, void* key, size_t size);

/*
    Frees up all data of hashMap.
*/
void killHashMap(hashMap* hashMap);

#endif
