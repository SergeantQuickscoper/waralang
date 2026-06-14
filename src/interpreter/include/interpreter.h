#ifndef INTERPRETER
#define INTERPRETER
#include <stdint.h>
#include <runtimeState.h>
#include <trie.h>

uint8_t interprer(runtimeState* mainRS, Trie* agentsTrie);

#endif