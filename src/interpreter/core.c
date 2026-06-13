#include <stdio.h>
#include <parser.h>
#include <string.h>

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

    uint8_t parserStatus = parsewl(sourceFile);

    if(parserStatus == 0){
        fprintf(stderr, "\nparser error\n");
    }
    else if(parserStatus == 1){
        fprintf(stderr, "\nparsing complete (≧∇≦)\n");
    }
    
    return 0;
}