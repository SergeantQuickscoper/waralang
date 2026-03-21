#include <stdio.h>
#include <messages.h>

void printEncoderErrorUsage(char* appName, char* argv0){
    printf("%s error! Incorrect usage!\n", appName);
    printf("Usage: %s <.txt> <.json>\n", argv0);
}

void printTextFileError(char* appName, char* argv1){
    printf("%s error! Text File Not Found!\n", appName);
    printf("The path: %s does not point to a valid text file.\n", argv1);
}
