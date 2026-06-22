#ifndef PARSER
#define PARSER
#include <stdio.h>
#include <stdint.h>
#include <trie.h>

uint8_t parsewl(FILE* sourceFile, Trie** agentsTriePtr, char** wmapFilePath, size_t* tickRate);

#endif