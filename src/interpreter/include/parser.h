#ifndef PARSER
#define PARSER
#include <stdio.h>
#include <stdint.h>
#include <trie.h>

/*
    Parser for the .wl file. Reads the file and saves all data in variables
    in memory.
*/

uint8_t parsewl(FILE* sourceFile, Trie** agntsTriePtr, char** wmapFilePath, size_t* tickRate);

#endif