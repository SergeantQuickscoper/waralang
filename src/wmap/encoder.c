#include <stdio.h>
#include <string.h>
#include "messages.h"
#include "encoderSub.h"
#include "uDynamInt.h"

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

    json_object* configObj = json_object_from_file(mapConfigPath);

    if(mapTextFile == NULL){
        printTextFileError(appName, argv[1]);
        return 1;
    }

    if(configObj == NULL){
        printTextFileError(appName, argv[2]);
        return 1;
    }

    
    mapData* map = initializeMapData(mapTextFile, configObj);

    fclose(mapTextFile);
    //free memory allocated to configObj. Should free up all variables derived from this too.
    json_object_put(configObj);

    if(map == NULL){
        return 1;
    }
    return 0;
}
