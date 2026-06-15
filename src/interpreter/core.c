#include <stdio.h>
#include <parser.h>
#include <string.h>
#include <interpreter.h>
#include <runtimeState.h>
#include <wmapDecoder.h>

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "winterp error: Incorrect usage!\n");
        fprintf(stderr, "Correct usage: wara <source.wl>\n");
        return 1;
    }

    char* sourcePath = argv[1];
    if(strlen(sourcePath)>=3 && strcmp(sourcePath + strlen(sourcePath) - 3, ".wl") != 0){
        fprintf(stderr, "winterp error: Incorrect usage!\n");
        fprintf(stderr, "Correct usage: wara <source.wl>\n");
        return 1;
    }

    FILE* sourceFile = fopen(sourcePath, "r");
    
    if(sourceFile == NULL){
        printf("winterp error! Text File Not Found!\n");
        printf("The path: %s does not point to a valid text file.\n", sourcePath);
    }

    Trie* agentsTrie;
    char* wmapFilePath;
    size_t tickRate;
    uint8_t parserStatus = parsewl(sourceFile, &agentsTrie, &wmapFilePath, &tickRate);

    if(parserStatus == 0){
        fprintf(stderr, "\nparser error\n");
        return 1;
    }
    else if(parserStatus == 1){
        fprintf(stderr, "\nparsing complete (≧∇≦)\n");
    }
    
    // TODO: make the path relative to .wl file
    runtimeState* mainRS = decodeWmap(wmapFilePath);

    uint8_t interprerStatus = interpret(mainRS, agentsTrie);
    if(interprerStatus == 0){
        fprintf(stderr, "\ninterpreter error\n");
        return 1;
    }

    return 0;
}