#ifndef INTERPRETERHELPERS
#define INTERPRETERHELPERS

#include <runtimeState.h>

void LLinsert(agentInst* inst, runtimeState* mainRS);

void pushCallStack(agentInst* inst, char** actualParams, size_t paramsLength, char* instructions, Agent* agent);

void popCallStack(agentInst* inst);

#endif