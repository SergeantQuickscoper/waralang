#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "messages.h"
#include "encoderSub.h"
#include "uDynamInt.h"

int main(int argc, char** argv){
    char* appName = "wcoder";

    if(argc != 3 && argc != 4){
        printEncoderErrorUsage(appName, argv[0]);
        return 1;
    }

    if(argc == 4 && strcmp(argv[3] + strlen(argv[3]) - 5, ".wmap") != 0){
        printEncoderErrorUsage(appName, argv[0]);
        fprintf(stderr, "Please mention a valid path to save the .wmap output"
        " including a <filename>.wmap!\n");
        return 1;
    }

    char* outPath = "./";
    if(argc == 4){
        outPath = argv[3];
    }
    char* mapTextPath = argv[1];
    char* mapConfigPath = argv[2];
    if(strcmp(mapTextPath + strlen(mapTextPath) - 4, ".txt") != 0 || strcmp(mapConfigPath +
        strlen(mapConfigPath) - 5, ".json") != 0){
        printEncoderErrorUsage(appName, argv[0]);
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

    uint8_t res = encodeData(outPath, map);

    if(res == 0){
        fprintf(stderr, "wcoder error: Error while writing wmap content.\n");
    }

    fprintf(stdout, "\n File successfully encoded!\n");
    return 0;
}
