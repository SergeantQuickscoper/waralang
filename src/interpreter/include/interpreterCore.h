#ifndef INTERPRETERCORE
#define INTERPRETERCORE

typedef struct {
    char* agentID;
    char* rawInstructions;
    char** params;
    size_t paramsLength;
} Agent;

#endif