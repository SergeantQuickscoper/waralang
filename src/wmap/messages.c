#include <stdio.h>
#include <messages.h>

void printEncoderErrorUsage(char* appName, char* argv0){
    printf("%s error! Incorrect usage!\n", appName);
    printf("Usage: %s <.txt> <.json>\n", argv0);
}
