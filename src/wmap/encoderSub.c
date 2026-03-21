#include <encoderSub.h>

uint8_t initializeMapData(FILE* txtFile){

    FILE* encodedwmap = fopen("./out.wmap", "wb");

    uDynamInt* height = createUDynamInt(sizeof(uint8_t));
    uDynamInt* width = createUDynamInt(sizeof(uint8_t));
    uDynamInt* widthCheck = createUDynamInt(sizeof(uint8_t));

    if(width == NULL || height == NULL || txtFile == NULL){
        fprintf(stderr, "wcoder error. Null references while initializing"
        "map data.");
        return 0;
    }
    mapData* obj = (mapData*)malloc(sizeof(mapData));
    obj->height = height;
    obj->width = width;
    size_t* heightAlloc = uDynamIntToSizeT(height);
    size_t* widthAlloc = uDynamIntToSizeT(width);
    // TODO: manage this overflow risk.
    obj->mapMatrix = (mapCell*)malloc(sizeof(mapCell) * (*heightAlloc) * (*widthAlloc));
    if(obj->mapMatrix == NULL){
        fprintf(stderr, "wmap error. Error allocating memory for map"
            "processing.");
        return 0;
    }

    int c; // could reduce this uint8_t
    mapCell* ptr = obj->mapMatrix;
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
    while((c = fgetc(txtFile)) != EOF){
        if(c > 127){
            fprintf(stderr, "Invalid non-ASCII character detected! Please"
                " fix your text files!\n Character seen: %c, Col: ", c);
            incrementValUDynamInt(widthCheck);
            incrementValUDynamInt(height);
            printNum(widthCheck);
            fprintf(stderr, ", Line: ");
            printNum(height);
            fprintf(stderr, "\n");
            return 0;
        }
        if(c == 10){
            height = incrementValUDynamInt(height);
            if(gotWidth == 0) gotWidth = 1;
            else{
                // check if width is diff than master width.
                if(isEqual(width, widthCheck) == 0){
                    fprintf(stderr, "wcoder error! Text file contains lines of"
                        " different sizes!, on Line: ");
                    printNum(height);
                    fprintf(stderr, "\n");
                    return 0;
                }

            }
            widthCheck = killUDynamicInt(widthCheck);
            widthCheck = createUDynamInt(sizeof(uint8_t));
        }
        else{
            if(gotWidth == 0) width = incrementValUDynamInt(width);
            else widthCheck = incrementValUDynamInt(widthCheck);
            ptr->symbol = c;
            // probably could set some bids based off of the json
            // file here (for reserved bids)
            ptr->bid = NULL;
        }
    }

    widthCheck = killUDynamicInt(widthCheck);
    if(width->size > 8 || height->size > 8){
        fprintf(stderr, "%s error. Unfortunately due to malloc constraints,"
        " you will only be able to use .txt files that have a maximum heightBytes or"
        " widthBytes of 8. Please use a smaller .txt file.\n");
        return 0;
    }

    // TODO: delegate this to some other function
    fwrite(&height->size, sizeof(uint8_t), sizeof(uint8_t), encodedwmap);
    fwrite(&width->size, sizeof(uint8_t), sizeof(uint8_t), encodedwmap);
    // TODO: fill in bidByteSize here
    fwrite(height->base, sizeof(uint8_t), height->size, encodedwmap);
    fwrite(width->base, sizeof(uint8_t), width->size, encodedwmap);

    return 1;
}
