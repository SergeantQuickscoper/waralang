#include <interpreterHelpers.h>
#include <stdio.h>
#include <stdlib.h>

void LLinsert(agentInst* inst, runtimeState* mainRS){
    inst->agentsLLNext = NULL;
    if(mainRS->aliveAgentsLL->head == NULL){
        mainRS->aliveAgentsLL->head = mainRS->aliveAgentsLL->tail = inst;
        inst->agentsLLPrev = NULL;
    }
    else{
        mainRS->aliveAgentsLL->tail->agentsLLNext = inst;
        inst->agentsLLPrev = mainRS->aliveAgentsLL->tail;
        mainRS->aliveAgentsLL->tail = inst;
    }
}

// paramsLength = 0 if not adding an agent
void pushCallStack(agentInst* inst, char** actualParams, size_t paramsLength, char* instructions, Agent* agent){
    // fprintf(stderr, "call stack instructions: %s\n", instructions);
    callStackNode* node = malloc(sizeof(callStackNode));
    node->instructions = instructions;
    node->programCounter = 0;
    
    // TODO: remove if else after hasmap accepts maxsize 1
    if(paramsLength > 0){
        node->params = createHashMap(paramsLength);
    }
    else{
        node->params = createHashMap(1);
    }
    for(size_t i = 0; i < paramsLength; i++){
        // fprintf(stderr, "param: %s\n", actualParams[i]);
        insertKey(node->params, agent->params[i], agent->paramNameLengths[i], actualParams[i]);
    }
    
    node->down = inst->callStackTop;
    inst->callStackTop = node;
}

void popCallStack(agentInst* inst){
    callStackNode* temp = inst->callStackTop;
    inst->callStackTop = temp->down;

    killHashMap(temp->params);
    free(temp);
}