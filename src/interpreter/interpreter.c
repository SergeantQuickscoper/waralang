#include <interpreter.h>
#include <stdio.h>
#include <runtimeState.h>

agentInst* instantiateAgent(Agent* agent, char** actualParams, size_t paramsLength){
    if(paramsLength != agent->paramsLength){
        if(paramsLength < agent->paramsLength){
            fprintf(stderr, "too few paramters for agent %s.", agent->agentID);
        }
        else{
            fprintf(stderr, "too many paramters for agent %s.", agent->agentID);
        }
        return NULL;
    }
    agentInst* inst = malloc(sizeof(agentInst));
    inst->instOf = agent;
    inst->actualParams = createTrie();

    for(size_t paramIdx = 0; paramIdx < paramsLength; paramIdx++){
        insertElementTrie(inst->actualParams, agent->params[paramIdx], actualParams[paramIdx]);
    }
}

uint8_t interpret(runtimeState* mainRS, Trie* agentsTrie){
    Agent* main = (Agent*)findElementTrie(agentsTrie, "main");

    if(main==agentsTrie->notEndPtr){
        fprintf(stderr, "main agent not found in .wl file.\n");
        return 0;
    }

    agentInst* mainInst =  instantiateAgent(main, NULL, 0);

    return 1;
}