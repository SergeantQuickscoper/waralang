#ifndef INTERPRETERCORE
#define INTERPRETERCORE

// for each agent defined in the .wl file
typedef struct {
    char* agentID;
    char* rawInstructions;
    char** params;
    size_t paramsLength;
} Agent;

#endif