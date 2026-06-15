#include <parser.h>
#include <string.h>
#include <stdlib.h>
#include <trie.h>
#include <runtimeState.h>

//always put EOF in delims array. delims array must end with '\0'
char* readWord(FILE* sourceFile, int* delims, int* delimFound, char** bufferPtr, size_t* bufferCapacityPtr){
    size_t bufferSize = 0;
    size_t bufferCapacity = *bufferCapacityPtr;
    char* buffer = *bufferPtr;

    while(1){
        int c = fgetc(sourceFile);
        if(c=='\n' || c==' '){
            continue;
        }
        int* delimPtr = delims;
        while(*delimPtr != '\0'){
            if(c==*delimPtr){
                *bufferPtr = buffer;
                *bufferCapacityPtr = bufferCapacity;
                
                *delimFound = c;
                char* res = malloc(sizeof(char) * (bufferSize+1));
                if(res==NULL){
                    fprintf(stderr, "memory allocation error\n");
                    return NULL;
                }
                memcpy(res, buffer, bufferSize);
                res[bufferSize] = '\0';
                return res;
            }
            delimPtr++;
        }

        buffer[bufferSize] = c;
        bufferSize++;
        if(bufferSize>bufferCapacity){
            bufferCapacity *= 2;
            buffer = realloc(buffer, sizeof(char) * bufferCapacity);
            if(buffer==NULL){
                fprintf(stderr, "memory allocation error\n");
                return NULL;
            }
        }
    }
}

uint8_t parsewl(FILE* sourceFile, Trie** agntsTriePtr, char** wmapFilePath, size_t* tickRate){
    char* wmap = malloc(sizeof(char)*1024);
    if(wmap==NULL){
        fprintf(stderr, "memory allocation error\n");
        return 0;
    }
    if(fscanf(sourceFile, "use %s", wmap) != 1){
        fprintf(stderr, ".wl must start with: \"use <path-to-wmap-file>\"\n");
        return 0;
    }
    *wmapFilePath = wmap;


    while(fgetc(sourceFile) == '\n');
    fseek(sourceFile, -1, SEEK_CUR);

    if(fscanf(sourceFile, "tickrate %zu", tickRate) != 1){
        fprintf(stderr, ".wl must contain \"tickrate <tickrate-amount>\" after use line.\n");
        return 0;
    }

    while(fgetc(sourceFile) == '\n');
    fseek(sourceFile, -1, SEEK_CUR);

    Trie* agentsTrie = createTrie();
    *agntsTriePtr = agentsTrie;

    int c = fgetc(sourceFile);

    if(c==EOF){
        return 1;
    }

    if(c != '!'){
        fprintf(stderr, "agent definition starts with '!'");
        return 0;
    }

    size_t charBufferCapacity = 1024;
    char* charBuffer = malloc(sizeof(char) * charBufferCapacity);
    if(charBuffer==NULL){
        fprintf(stderr, "memory allocation error\n");
        return 0;
    }

    size_t paramBufferCapacity = 1024;
    char** paramBuffer = malloc(sizeof(char*) * paramBufferCapacity);
    if(paramBuffer==NULL){
        fprintf(stderr, "memory allocation error\n");
        return 0;
    }

    int agentIdDelims[] = {'(', EOF, '\0'};
    int paramDelims[] = {')', ',', EOF, '\0'};
    int instructionDelims[] = {'!', EOF, '\0'};
    int delimFound = '\0';

    while(delimFound != EOF){
        Agent* agent = malloc(sizeof(Agent));
        if(agent==NULL){
            fprintf(stderr, "memory allocation error\n");
            return 0;
        }

        // read agentID (!<agent-id>(parameters):)
        agent->agentID = readWord(sourceFile, agentIdDelims, &delimFound, &charBuffer, &charBufferCapacity);
        
        void* trieStatus = insertElementTrie(agentsTrie, agent->agentID, agent);
        if(trieStatus == (void*)-1){
            return 0;
        }
        if(trieStatus != agentsTrie->notEndPtr){
            fprintf(stderr, "multiple agents cannot have the same name.\n");
            return 0;
        }


        if(delimFound == EOF){
            fprintf(stderr, "reached end of file while reading agent name.\n");
            return 0;
        }

        // read parameters
        size_t paramBuffereSize = 0;
        agent->params = NULL;
        if(fgetc(sourceFile) != ')'){
            fseek(sourceFile, -1, SEEK_CUR);
            while(1){
                paramBuffer[paramBuffereSize] = readWord(sourceFile, paramDelims, &delimFound, &charBuffer, &charBufferCapacity);
                paramBuffereSize++;
                if(delimFound == EOF){
                    fprintf(stderr, "reached end of file while reading paramter %zu of agent %s.\n", paramBuffereSize, agent->agentID);
                    return 0;
                }
                
                if(delimFound == ')'){
                    break;
                }
                
                if(paramBuffereSize > paramBufferCapacity){
                    paramBufferCapacity *= 2;
                    paramBuffer = realloc(paramBuffer, sizeof(char*) * paramBufferCapacity);
                    if(paramBuffer==NULL){
                        fprintf(stderr, "memory allocation error\n");
                        return 0;
                    }
                }
            }
            
            agent->params = malloc(sizeof(char*) * paramBuffereSize);
            if(agent->params==NULL){
                fprintf(stderr, "memory allocation error\n");
                return 0;
            }
            memcpy(agent->params, paramBuffer, sizeof(char*) * paramBuffereSize);
        }
        agent->paramsLength = paramBuffereSize;

        //read code
        size_t codeSize;
        agent->rawInstructions = readWord(sourceFile, instructionDelims, &delimFound, &charBuffer, &charBufferCapacity);
    }

    free(charBuffer);
    free(paramBuffer);

    return 1;
}