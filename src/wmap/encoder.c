#include <stdio.h>
#include <string.h>
#include "messages.h"
#include "encoderSub.h"
#include "uDynamInt.h"
#include <json-c/json.h>

int main(int argc, char** argv){
    char* appName = "wcoder";

    if(argc != 3){
        printEncoderErrorUsage(appName, argv[0]);
        return 1;
    }

    char* mapTextPath = argv[1];
    char* mapConfigPath = argv[2];
    if(strcmp(mapTextPath + strlen(mapTextPath) - 4, ".txt") != 0 || strcmp(mapConfigPath +
        strlen(mapConfigPath) - 5, ".json") != 0){
        printEncoderErrorUsage(appName,argv[0]);
        return 1;
    }

    FILE* mapTextFile = fopen(mapTextPath, "r");

    // json-c magic should handle this
    FILE* mapConfigFile = fopen(mapConfigPath, "r");

    if(mapTextFile == NULL){
        printTextFileError(appName, argv[1]);
        return 1;
    }

    uint8_t mapStatus = initializeMapData(mapTextFile);

    return ~mapStatus;
}
