#include <encoderSub.h>

/*
    Currently the dfs only checks the vertical and horizontal cells for the bid
    symbol to be considered under building. This should probably be upto user
    config though so leaving a TODO: modify the movesX and movesY arrays to
    accomodate user configs and design how said configs are designed.
*/
void dfs(mapData* mapData, uDynamInt* currInd, size_t height, size_t width,
    size_t currX, size_t currY){
    if((mapData->mapMatrix + currY * width + currX)->symbol !=
            mapData->buildingPlaceHolder){
        return;
    }
    int8_t movesX[4] = {1, 0, -1, 0};
    int8_t movesY[4] = {0, -1, 0, 1};
    (mapData->mapMatrix + currY * width + currX)->symbol = 0; // arbitrary
    (mapData->mapMatrix + currY * width + currX)->bid = currInd;
    // TODO: do something about this magic number too when applying config.
    // and the hardcoded invalid moves
    for(uint8_t i = 0; i < 4; i++){
        if(currX == 0 && movesX[i] == -1) continue;
        if(currX == width - 1 && movesX[i] == 1) continue;
        if(currY == 0 && movesY[i] == -1) continue;
        if(currY == height - 1 && movesY[i] == 1) continue;
        dfs(mapData, currInd, height, width, currX + movesX[i], currY + movesY[i]);
    }
}
uint8_t fillBuildings(mapData* mapData){
    if(mapData == NULL || mapData->height == NULL || mapData->width == NULL){
        fprintf(stderr, "wmap encoder error! uninitialized values during DFS!"
        );
        return 0;
    }
    size_t* heightS = uDynamIntToSizeT(mapData->height);
    size_t* widthS = uDynamIntToSizeT(mapData->width);
    uDynamInt* bidCount = createUDynamInt(sizeof(uint8_t));
    for(size_t i = 0; i < *heightS; i++){
        for(size_t j = 0; j < *widthS; j++){
            if((mapData->mapMatrix + i * (*widthS) + j)->symbol ==
            mapData->buildingPlaceHolder){
                bidCount = incrementValUDynamInt(bidCount);
                uDynamInt* currInd = copyUDynamInt(bidCount);
                dfs(mapData, currInd, *heightS, *widthS, j, i);
            }
        }
    }
    mapData->bidCount = bidCount;
    free(heightS);
    free(widthS);
}

uint8_t initializeTextData(FILE* txtFile, mapData* map){
    uDynamInt* height = createUDynamInt(sizeof(uint8_t));
    uDynamInt* width = createUDynamInt(sizeof(uint8_t));
    uDynamInt* widthCheck = createUDynamInt(sizeof(uint8_t));

    if(width == NULL || height == NULL || txtFile == NULL){
        fprintf(stderr, "wcoder error. Null references while initializing"
        "map data.");
        return 0;
    }

    int c; // could reduce this uint8_t
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
        }
    }

    widthCheck = killUDynamicInt(widthCheck);
    if(width->size > 8 || height->size > 8){
        fprintf(stderr, "wcoder error. Unfortunately due to malloc constraints,"
        " you will only be able to use .txt files that have a maximum heightBytes or"
        " widthBytes of 8. Please use a smaller .txt file.\n");
        return 0;
    }
    size_t* heightAlloc = uDynamIntToSizeT(height);
    size_t* widthAlloc = uDynamIntToSizeT(width);
    if(heightAlloc == NULL || widthAlloc == NULL){
        fprintf(stderr, "wmap error. Error with uDynamInt and map dimensions");
        return 0;
    }
    // TODO: manage this overflow risk from multiplying size_t's
    map->height = height;
    map->width = width;
    map->mapMatrix = (mapCell*)malloc(sizeof(mapCell) * (*heightAlloc)
     * (*widthAlloc));
    if(map->mapMatrix == NULL){
        fprintf(stderr, "wmap error. Error allocating memory for map"
            " processing.");
        return 0;
    }
    size_t ptr = 0;
    fseek(txtFile, 0, SEEK_SET);
    clearerr(txtFile);
    while((c = fgetc(txtFile)) != EOF){
        if(c == 10) continue;
        mapCell* currCell = map->mapMatrix + ptr++;
        currCell->symbol = c;
    }
    fillBuildings(map);
    return 1;
}


uint8_t validateJsonObject(json_object* obj, enum json_type desiredType, char* name){
    if(obj == NULL){
        fprintf(stderr, "wcoder error. ");
        fprintf(stderr, "%s", name);
        fprintf(stderr, " not found, or is NULL\n");
        return 0;
    }
    if(!json_object_is_type(obj, desiredType)){
        fprintf(stderr, "wcoder error. ");
        fprintf(stderr, "%s", name);
        fprintf(stderr, " is of wrong type\n");
        return 0;
    }
    return 1;
}

//incomplete function
uint8_t initializeConfigData(json_object* configObj, mapData* map){
    if(!validateJsonObject(configObj, json_type_object, "root")) return 0;

    json_object* spawnObj = json_object_object_get(configObj, "spawn");
    if(!validateJsonObject(spawnObj, json_type_object, "spawn")) return 0;

    json_object* spawnXObj = json_object_object_get(spawnObj, "x");
    if(!validateJsonObject(spawnXObj, json_type_int, "spawnX")) return 0;

    json_object* spawnYObj = json_object_object_get(spawnObj, "y");
    if(!validateJsonObject(spawnYObj, json_type_int, "spawnY")) return 0;

    json_object* buildingsArrObj = json_object_object_get(configObj, "buildings");
    if(!validateJsonObject(buildingsArrObj, json_type_array, "buildings array")) return 0;
    array_list* buildingsArr = json_object_get_array(buildingsArrObj);

    const char* ALLOWEDOPCODES[] = {
                            "NOP",
                            "MEM",
                            "REG",
                            "ADD",
                            "MULT",
                            "LOAD",
                            "STORE",
                            "COMP",
                            "STDIN",
                            "STDOUT_ASCII",
                            "STDOUT_INT"
    };
    const uint8_t ALLOWEDOPCODESLENGTH = sizeof(ALLOWEDOPCODES)/sizeof(ALLOWEDOPCODES[0]);

    map->buildings = malloc(sizeof(bidMap) * buildingsArr->length);

    //same value as i, just in uDynamInt to set bid
    uDynamInt* iuDynam = createUDynamInt(sizeof(uint8_t));

    for(size_t i = 0; i<buildingsArr->length; i++){
        json_object* buildingObj = (json_object*)array_list_get_idx(buildingsArr, i);
        //TODO: add building number to title for easier debugging
        if(!validateJsonObject(buildingObj, json_type_object, "building")) return 0;

        //this foreach is there just to get the only 1 element
        json_object_object_foreach(buildingObj, buildingType, buildingDetailsObj){
            if(!validateJsonObject(buildingDetailsObj, json_type_object, "building details")) return 0;

            if(!strcmp(buildingType, "FUNC")){
                //copy iuDynam to bid
                (map->buildings + i)->bid = createUDynamInt(iuDynam->size);
                memcpy((map->buildings + i)->bid->base, iuDynam->base, sizeof(uint8_t) * iuDynam->size);

                uDynamInt* opcodesCount = createUDynamInt(sizeof(uint8_t));
                json_object* opcodeListObj = json_object_object_get(buildingDetailsObj, "opcodeList");
                if(!validateJsonObject(opcodeListObj, json_type_array, "building opcodeList"))return 0;
                array_list* opcodeList = json_object_get_array(opcodeListObj);

                for(size_t i = 0; i<opcodeList->length; i++){
                    opcodesCount = incrementValUDynamInt(opcodesCount);
                }
                (map->buildings + i)->opcodeCount = opcodesCount;

                (map->buildings + i)->opcodes = malloc( opcodeList->length * sizeof(uint8_t) );

                for(size_t j = 0; j<opcodeList->length; j++){
                    //as UINT8_MAX is a reserved opcode
                    uint8_t opcode = 0;
                    uint8_t opcodeMatched = 0;

                    json_object* opcodeObj = (json_object*)array_list_get_idx(opcodeList, j);
                    if(!validateJsonObject(opcodeObj, json_type_string, "opcode")) return 0;
                    const char* opcodestr = json_object_get_string(opcodeObj);

                    for(uint8_t k = 0; k<ALLOWEDOPCODESLENGTH; k++){
                        if(!strcmp( opcodestr, ALLOWEDOPCODES[k])){
                            opcode = k;
                            opcodeMatched = 1;
                            break;
                        }
                    }
                    if(!opcodeMatched){
                        fprintf(stderr, "wcoder error. Undefined opcode used in FUNC ");
                        printNum(iuDynam);
                        fprintf(stderr, "\n");
                        return 0;
                    }
                    if(opcode == 1 || opcode == 2){
                        fprintf(stderr, "wcoder error. REG or MEM opcode used in building ");
                        printNum(iuDynam);
                        fprintf(stderr, "\n");
                        return 0;
                    }
                    (map->buildings + i)->opcodes[j] = opcode;
                }
            }
            break;
        }
        iuDynam = incrementValUDynamInt(iuDynam);
    }
    json_object_put(configObj);
    killUDynamicInt(iuDynam);
    return 1;
}

mapData* initializeMapData(FILE* mapTextFile, json_object* configObj){
    mapData* map = (mapData*)malloc((sizeof(mapData)));
    map->buildingPlaceHolder = '$'; // TODO: actually detect this in the below func.
    uint8_t configStatus = initializeConfigData(configObj, map);
    uint8_t mapStatus = initializeTextData(mapTextFile, map);
    if(mapStatus == 0 || configStatus == 0) return NULL;
    return map;
}