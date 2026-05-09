#include <stdio.h>

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "winterp error: Incorrect usage!\n");
        fprintf(stderr, "Correct usage: wara <source.wl>\n");
        return 1;
    }
    return 0;
}