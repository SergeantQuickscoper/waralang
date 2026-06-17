#include <interpreter.h>
#include <stdio.h>
#include <runtimeState.h>
#include <stdlib.h>

agentInst* spawnAgent(Agent* agent, char** actualParams, size_t paramsLength, runtimeState* mainRS){
    if(mainRS->spawnCell->activeAgent != NULL){
        fprintf(stderr, "new agent spawned when spawn cell is already occupied.");
        return NULL;
    }
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
        void* res = insertElementTrie(inst->actualParams, agent->params[paramIdx], actualParams[paramIdx]);
        if(res==(void*)-1){
            fprintf(stderr, "in parameter names of agent %s", agent->agentID);
            return NULL;
        }
        if(res != inst->actualParams->notEndPtr){
            fprintf(stderr, "multiple parameters have same name in definition of agent %s", agent->agentID);
            return NULL;
        }
    }

    inst->currLoc = mainRS->spawnCell;
    mainRS->spawnCell->activeAgent = inst;

    inst->currDir = mainRS->spawnDirection;
    inst->programCounter = 0;

    inst->agentsLLNext = NULL;
    if(mainRS->aliveAgentsLL->head == NULL){
        mainRS->aliveAgentsLL->head = mainRS->aliveAgentsLL->tail = inst;
    }
    else{
        mainRS->aliveAgentsLL->tail->agentsLLNext = inst;
        mainRS->aliveAgentsLL->tail = inst;
    }

    return inst;
}

uint8_t interpret(runtimeState* mainRS, Trie* agentsTrie){
    size_t tick = 0;

    Agent* main = (Agent*)findElementTrie(agentsTrie, "main");
    
    if(main==agentsTrie->notEndPtr){
        fprintf(stderr, "main agent not found in .wl file.\n");
        return 0;
    }
    
    agentInst* mainInst =  spawnAgent(main, NULL, 0, mainRS);
    
    if(mainInst == NULL){
        return 0;
    }
    return 1;
}