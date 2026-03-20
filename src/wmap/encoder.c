#include <stdio.h>
#include <string.h>
#include "messages.h"
int main(int argc, char** argv){
    char* appName = "wcoder";

    if(argc != 3){
        printErrorUsage(appName, argv[0]);
        return 1;
    }
    if(strcmp(argv[1] + strlen(argv[1]) - 4, ".txt") != 0 || strcmp(argv[2] +
        strlen(argv[2]) - 5, ".json") != 0){
        printErrorUsage(appName,argv[0]);
        return 1;
    }
}
