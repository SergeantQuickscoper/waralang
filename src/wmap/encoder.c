#include <stdio.h>
#include <string.h>
#include "messages.h"
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

    // TODO: lowkey convert this into a subroutine inside encoderSub.c
    uDynamInt* height = createUDynamInt(sizeof(uint8_t));
    uDynamInt* width = createUDynamInt(sizeof(uint8_t));
    uDynamInt* widthCheck = createUDynamInt(sizeof(uint8_t));

    FILE* mapTextFile = fopen(mapTextPath, "r");

    // json-c magic should handle this
    FILE* mapConfigFile = fopen(mapConfigPath, "r");

    FILE* encodedwmap = fopen("./out.wmap", "wb");

    if(mapTextFile == NULL){
        printTextFileError(appName, argv[1]);
        return 1;
    }

    int c; // could maybe reduce this to int_8
    uint8_t gotWidth = 0;
    /*
        getting newline would be slightly different in UNIX and Windows due to
        LF and CRLF techiniques, the current implementation supports only LF
        so remove this comment if the TODO is complete
        TODO: add CRLF support or convert into CRLF

        \n - ASCII Decimal 10
        \r - ASCII Decimal 13

        NOTE: .txt files MUST end with a \n character too or this could cause
        issues.

        NOTE: Every Line print is currently flawed as I am yet to create a
        method to print 256 bit integers. Rn only the base is printed as a way
    */
    while((c = fgetc(mapTextFile)) != EOF){
        if(c > 127){
            fprintf(stderr, "Invalid non-ASCII character detected! Please"
                " fix your text files!\n Character seen: %c, Col: %d, on"
                " Line %d", c, *(widthCheck->base) + 1, *(height->base) + 1);
            return 1;
        }
        if(c == 10){
            height = incrementValUDynamInt(height);
            if(gotWidth == 0) gotWidth = 1;
            else{
                // check if width is diff than master width.
                if(isEqual(width, widthCheck) == 0){
                    fprintf(stderr, "%s error! Text file contains lines of"
                        " different sizes!, on Line: %d\n", appName, *(height->base) + 1);
                    return 1;
                }

            }
            widthCheck = killUDynamicInt(widthCheck);
            widthCheck = createUDynamInt(sizeof(uint8_t));
        }
        else if(gotWidth == 0) width = incrementValUDynamInt(width);
        else widthCheck = incrementValUDynamInt(widthCheck);
    }
    widthCheck = killUDynamicInt(widthCheck);

    fwrite(&height->size, sizeof(uint8_t), sizeof(uint8_t), encodedwmap);
    fwrite(&width->size, sizeof(uint8_t), sizeof(uint8_t), encodedwmap);
    // TODO: fill in bidByteSize here
    fwrite(height->base, sizeof(uint8_t), height->size, encodedwmap);
    fwrite(width->base, sizeof(uint8_t), width->size, encodedwmap);
}
