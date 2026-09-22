#ifndef INTERPRETER
#define INTERPRETER
#include <stdint.h>
#include <runtimeState.h>
#include <trie.h>

/*
    interpreter for after all structs have been initialized with data.
*/

uint8_t interpret(runtimeState* mainRS, Trie* agentsTrie);

#endif