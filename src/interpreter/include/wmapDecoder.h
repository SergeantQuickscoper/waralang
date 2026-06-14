#ifndef WMAPDECODER
#define WMAPDECODER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <waralibs.h>
#include <runtimeState.h>

runtimeState* decodeWmap(char* wmapPath);

// TODO: implement!
void freeDecodedWmap(runtimeState* state);

#endif