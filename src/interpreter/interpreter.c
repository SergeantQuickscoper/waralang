#include <interpreter.h>
#include <interpreterHelpers.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    this function might just be the best code I've ever written
    when called externally, returns next char which would appear after substitutions.
    rest of the characters, except ',' are ret
    returns -1 upon error, '\0' if execution is finished.
*/
int readChar(agentInst* inst, runtimeState* mainRS){
    if(inst->callStackTop == NULL){
        return '\0';
    }
    if(inst->callStackTop->instructions[inst->callStackTop->programCounter] == '\0'){
        popCallStack(inst);
        return readChar(inst, mainRS);
    }

    char res = inst->callStackTop->instructions[inst->callStackTop->programCounter];
    inst->callStackTop->programCounter++;
    if(res == '{'){
        size_t bufferSize = 0, bufferCapacity = 128;
        char* buffer = malloc(sizeof(char) * bufferCapacity);

        // read name of agent/parameter into buffer
        while(res!='}' && res!='('){
            res = buffer[bufferSize++] = readChar(inst, mainRS);
            if(bufferSize+1 > bufferCapacity){
                bufferCapacity *= 2;
                buffer = realloc(buffer, sizeof(char) * bufferCapacity);
            }
        }
        bufferSize--; // bufferSize includes '}' / '('
        buffer[bufferSize] = '\0';

        char** actualParams;
        size_t paramsLen;
        char* instructions;
        Agent* agent;

        // agent substitution
        if(res == '('){
            agent = findElementTrie(mainRS->agentsTrie, buffer);
            actualParams = malloc(sizeof(char*) * agent->paramsLength);
            paramsLen = agent->paramsLength;
            instructions = agent->rawInstructions;

            size_t paramsId = 0;
            res = readChar(inst, mainRS); // first character after '('
            while(res != '}'){
                // push first character into buffer
                bufferSize = 1;
                buffer[0] = res;
                res = '\0';
                // last parameter is just ')' terminated instead of ',' terminated.
                // No special treatment required!
                while(res != ',' && res != ')'){
                    res = buffer[bufferSize++] = readChar(inst, mainRS);
                    if(bufferSize > bufferCapacity){
                        bufferCapacity *= 2;
                        buffer = realloc(buffer, sizeof(char) * bufferCapacity);
                    }
                }
                bufferSize--; // bufferSize includes ','
                if(bufferSize == 1 && buffer[0]=='$'){
                    bufferSize = 0;
                }

                res = readChar(inst, mainRS); // skip ',' / ')'
                actualParams[paramsId] = malloc(sizeof(char) * (bufferSize+1));
                memcpy(actualParams[paramsId], buffer, bufferSize);
                actualParams[paramsId][bufferSize] = '\0';
                // fprintf(stderr, "%zu %zu %s\n", paramsId, bufferSize, actualParams[paramsId]);
                paramsId++;
            }
            if(paramsId != paramsLen){
                fprintf(stderr, "wrong number of parameters in call for agent %s", agent->agentID);
                return -1;
            }
        }
        // parameter substitution
        else if(res == '}'){
            agent = NULL;
            actualParams = NULL;
            paramsLen = 0;
            instructions = findKey(inst->callStackTop->params, buffer, bufferSize);
        }
        pushCallStack(inst, actualParams, paramsLen, instructions, agent);
        // skip '}'
        res = readChar(inst, mainRS);
    }
    else if(res == '?'){

    }
    return res;
}


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
    inst->callStackTop = NULL;
    pushCallStack(inst, actualParams, paramsLength, agent->rawInstructions, agent);

    inst->currLoc = mainRS->spawnCell;
    mainRS->spawnCell->activeAgent = inst;
    inst->currDir = mainRS->spawnDirection;

    LLinsert(inst, mainRS);

    return inst;
}

uint8_t move(agentInst* inst, runtimeState* mainRS){
    mapCell* nextLoc;
    if(inst->currDir == UP){
        nextLoc = inst->currLoc - mainRS->map->width;
        if(nextLoc < mainRS->map->mapMatrix){
            fprintf(stderr, "instance of agent %s went out of bounds\n", inst->instOf->agentID);
            return 0;
        }
    }
    else if(inst->currDir == LEFT){
        nextLoc = inst->currLoc - 1;
        if((inst->currLoc - mainRS->map->mapMatrix) % mainRS->map->width == 0){
            fprintf(stderr, "instance of agent %s went out of bounds\n", inst->instOf->agentID);
            return 0;
        }
    }
    else if(inst->currDir == DOWN){
        nextLoc = inst->currLoc + mainRS->map->width;
        if(nextLoc >= mainRS->map->mapMatrix + mainRS->map->height*mainRS->map->width){
            fprintf(stderr, "instance of agent %s went out of bounds\n", inst->instOf->agentID);
            return 0;
        }
    }
    else if(inst->currDir == RIGHT){
        nextLoc = inst->currLoc + 1;
        if((nextLoc - mainRS->map->mapMatrix) % mainRS->map->width == 0){
            fprintf(stderr, "instance of agent %s went out of bounds\n", inst->instOf->agentID);
            return 0;
        }
    }

    if(nextLoc->bid == mainRS->reservedBids.collidersBid){
        // turn left
        if(inst->currDir == UP){
            inst->currDir = LEFT;
        }
        else if(inst->currDir == LEFT){
            inst->currDir = DOWN;
        }
        else if(inst->currDir == DOWN){
            inst->currDir = RIGHT;
        }
        else if(inst->currDir == RIGHT){
            inst->currDir = UP;
        }
    }
    else{
        inst->currLoc = nextLoc;
    }
    return 1;
}

uint8_t processTickAgent(agentInst* inst, runtimeState* mainRS){
    // for debugging:
    // fprintf(stderr, "dir: %u ", inst->currDir);
    // fprintf(stderr, "symbol: %c ", inst->currLoc->symbol);
    // fprintf(stderr, "x: %zu ", (inst->currLoc - mainRS->map->mapMatrix)%mainRS->map->width);
    // fprintf(stderr, "y: %zu\n", (inst->currLoc - mainRS->map->mapMatrix)/mainRS->map->width);

    if(inst->currLoc->bid == mainRS->reservedBids.traversablesBid){
        uint8_t moveStatus = move(inst, mainRS);
        if(moveStatus == 0){
            return 0;
        }
    }
    else if(inst->currLoc->bid == mainRS->reservedBids.junctionsBid){
        int dir = readChar(inst, mainRS);

        if(dir == -1){
            return 0;
        }

        if(dir == '^'){
            inst->currDir = UP;
        }
        else if(dir == '<'){
            inst->currDir = LEFT;
        }
        else if(dir == 'v'){
            inst->currDir = DOWN;
        }
        else if(dir == '>'){
            inst->currDir = RIGHT;
        }
        else{
            fprintf(stderr, "expected direction character('^' / '<' / 'v' / '>')");
            fprintf(stderr, " for instance of agent %s", inst->instOf->agentID);
            fprintf(stderr, " but encountered %c(ASCII %u)\n", dir, dir);
            return 0;
        }

        move(inst, mainRS);
    }
    
    else{
        fprintf(stderr, "in building, to be implemented\n");
        return 0;
        // TODO for @SamyakJainABCD: building execution
    }
}

uint8_t processTick(runtimeState* mainRS){
    agentInst* iteratorLL = mainRS->aliveAgentsLL->head;
    while(iteratorLL != NULL){
        uint8_t processTickAgentStatus = processTickAgent(iteratorLL, mainRS);
        if(processTickAgentStatus == 0){
            return 0;
        }
    }
}

uint8_t interpret(runtimeState* mainRS){
    size_t tick = 0;

    Agent* main = (Agent*)findElementTrie(mainRS->agentsTrie, "main");

    if(main==mainRS->agentsTrie->notEndPtr){
        fprintf(stderr, "main agent not found in .wl file.\n");
        return 0;
    }

    agentInst* mainInst =  spawnAgent(main, NULL, 0, mainRS);

    if(mainInst == NULL){
        return 0;
    }

    while(1){
        uint8_t processTickStatus = processTick(mainRS);
        if(processTickStatus == 0){
            return 0;
        }

        if(mainRS->aliveAgentsLL->head == NULL){
            return 1;
        }
    }

    return 0;
}