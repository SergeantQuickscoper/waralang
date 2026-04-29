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
            mapData->buildingSymbol){
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

void printCurrMapMatrix(mapData* mapData){
    if(mapData == NULL){
        fprintf(stderr, "wmap encoder error while printing map!"
        );
    }
    size_t* heightS = uDynamIntToSizeT(mapData->height);
    size_t* widthS = uDynamIntToSizeT(mapData->width);
    size_t counter = 0;
    FILE* out = fopen("./currMapMatrix.txt", "w"); // hard
    for(size_t i = 0; i < *heightS; i++){
        for(size_t j = 0; j < *widthS; j++){
            if(counter == *widthS){
                counter = 0;
                char new = '\n';
                fwrite(&new, sizeof(char), 1, out);
            }
            if((mapData->mapMatrix + *widthS * i + j)->symbol == 0){
                uDynamInt* bid = (mapData->mapMatrix + *widthS * i + j)->bid;
                size_t* temp = uDynamIntToSizeT(bid);
                char toPrint = 62 + *temp;
                fwrite(&toPrint, sizeof(char), 1, out);
                free(temp);
            }
            else{
                fwrite(&((mapData->mapMatrix + *widthS * i + j)->symbol),
                sizeof(char), 1, out);
            }
            counter++;
        }
    }
    fclose(out);
    free(heightS);
    free(widthS);
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
            mapData->buildingSymbol){
                bidCount = incrementValUDynamInt(bidCount);
                uDynamInt* currInd = copyUDynamInt(bidCount);
                dfs(mapData, currInd, *heightS, *widthS, j, i);
            }
            else{
                /*
                  TODO: here would be a good place to decide whether a symbol
                  is a traversable, collider or junction (reserved bid
                  implementation).
                */
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
        fprintf(stderr, "wcoder error. Unfortunately due to malloc constraints"
        " you will only be able to use .txt files that have a maximum "
        " heightBytes or widthBytes of 8. Please use a smaller .txt file.\n");
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
        fprintf(stderr, "wcoder error. In cofig file: ");
        fprintf(stderr, "%s", name);
        fprintf(stderr, " not found, or is NULL\n");
        return 0;
    }
    if(!json_object_is_type(obj, desiredType)){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "%s", name);
        fprintf(stderr, " is of wrong type\n");
        return 0;
    }
    return 1;
}

//incomplete function
uint8_t initializeConfigData(json_object* configObj, mapData* map){
    if(!validateJsonObject(configObj, json_type_object, "root")) return 0;
    
    

    //spawn
    json_object* spawnObj = json_object_object_get(configObj, "spawn");
    if(!validateJsonObject(spawnObj, json_type_object, "spawn")) return 0;

    //TODO: validate spawnX and spawnY with height and width of map

    json_object* spawnXObj = json_object_object_get(spawnObj, "x");
    if(!validateJsonObject(spawnXObj, json_type_int, "spawnX")) return 0;
    int64_t spawnXInt64T = json_object_get_int64(spawnXObj);
    if(spawnXInt64T<0){
        fprintf(stderr, "wcoder error. In config file: spawnX cannot be negative");
    }
    size_t spawnXSizeT = (size_t)spawnXInt64T;
    uDynamInt* spawnXUDynam = sizeTToUDynamInt(spawnXSizeT);
    map->spawnX = spawnXUDynam;

    json_object* spawnYObj = json_object_object_get(spawnObj, "y");
    if(!validateJsonObject(spawnYObj, json_type_int, "spawnY")) return 0;
    int64_t spawnYInt64T = json_object_get_int64(spawnYObj);
    if(spawnYInt64T<0){
        fprintf(stderr, "wcoder error. In config file: spawnY cannot be negative");
    }
    size_t spawnYSizeT = (size_t)spawnYInt64T;
    uDynamInt* spawnYUDynam = sizeTToUDynamInt(spawnYSizeT);
    map->spawnY = spawnYUDynam;

    json_object* spawnDirectionObj = json_object_object_get(spawnObj, "direction");
    if(!validateJsonObject(spawnDirectionObj, json_type_string, "spawnDirection")) return 0;
    const char* spawnDirectionString = json_object_get_string(spawnDirectionObj);
    if(strcmp(spawnDirectionString,"<") && strcmp(spawnDirectionString,">") &&
        strcmp(spawnDirectionString, "^") && strcmp(spawnDirectionString, "v")){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "spawn direction can only be one of these: ");
        fprintf(stderr, "<: left, >: right, v: down, ^: up\n");
        return 0;
    }
    char spawnDirectionChar = spawnDirectionString[0];
    map->spawnDirection = spawnDirectionChar;


    //word count
    json_object* wordSizeObj = json_object_object_get(configObj, "wordSize");
    if(!validateJsonObject(wordSizeObj, json_type_object, "wordSize")) return 0;
    
    json_object* baseAddressSizeObj = json_object_object_get(wordSizeObj, "baseAddressSize");
    if(!validateJsonObject(baseAddressSizeObj, json_type_int, "baseAddressSize")) return 0;
    int64_t baseAddressSizeInt64 = json_object_get_int64(baseAddressSizeObj);
    if(baseAddressSizeInt64<0){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "baseAddressSize cannot be negative.\n");
        return 0;
    }
    size_t baseAddressSizeSizeT = (size_t)baseAddressSizeInt64;
    uDynamInt* baseAddressSizeUDynam = sizeTToUDynamInt(baseAddressSizeSizeT);
    map->baseAddressSize = baseAddressSizeUDynam;

    json_object* subAddressSizeObj = json_object_object_get(wordSizeObj, "subAddressSize");
    if(!validateJsonObject(subAddressSizeObj, json_type_int, "subAddressSize")) return 0;
    int64_t subAddressSizeInt64 = json_object_get_int64(subAddressSizeObj);
    if(subAddressSizeInt64<0){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "subAddressSize cannot be negative.\n");
        return 0;
    }
    //because we are storing storage size of `MEM` buildings in an int64
    if(subAddressSizeInt64>63){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "subAddressSize cannot be greater than 63.\n");
        return 0;
    }
    size_t subAddressSizeSizeT = (size_t)subAddressSizeInt64;
    uDynamInt* subAddressSizeUDynam = sizeTToUDynamInt(subAddressSizeSizeT);
    map->subAddressSize = subAddressSizeUDynam;
    size_t subAddressMax = (1<<subAddressSizeSizeT)-1;

    //symbols
    json_object* buildingSymbolObj = json_object_object_get(configObj, "buildingSymbol");
    if(!validateJsonObject(buildingSymbolObj, json_type_string, "buildingSymbol")) return 0;
    const char* buildingSymbolStr = json_object_get_string(buildingSymbolObj);
    size_t buildingSymbolLen = sizeof(buildingSymbolStr)/sizeof(buildingSymbolStr[0]);
    if(buildingSymbolLen!=1){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "buildingSymbol should be 1 character string.\n");
        return 0;
    }
    map->buildingSymbol = buildingSymbolStr[0];

    json_object* junctionSymbolObj = json_object_object_get(configObj, "junctionSymbol");
    if(!validateJsonObject(junctionSymbolObj, json_type_string, "junctionSymbol")) return 0;
    const char* junctionSymbolStr = json_object_get_string(junctionSymbolObj);
    size_t junctionSymbolLen = sizeof(junctionSymbolStr)/sizeof(junctionSymbolStr[0]);
    if(junctionSymbolLen!=1){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "junctionSymbol should be 1 character string.\n");
        return 0;
    }
    if(junctionSymbolStr[0] == buildingSymbolStr[0]){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "building symbol and junction symbol cannot be same.\n");
        return 0;
    }
    map->junctionSymbol = junctionSymbolStr[0];


    json_object* collidersObj = json_object_object_get(configObj, "colliders");
    if(!validateJsonObject(collidersObj, json_type_array, "colliders"));
    array_list* collidersArr = json_object_get_array(collidersObj);
    if(collidersArr->length == 0){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "There cannot be 0 colliders.\n");
        return 0;
    }
    if(collidersArr->length > 255){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "There cannot be more than 255 colliders.");
        fprintf(stderr, "(why do you even need that many? ");
        fprintf(stderr, "there's only 256 ascii characters)\n");
        return 0;
    }
    map->collidersBytes = collidersArr->length;
    map->colliders = malloc(collidersArr->length * sizeof(char));
    
    for(size_t i = 0; i<collidersArr->length; i++){
        json_object* colliderObj = (json_object*)array_list_get_idx(collidersArr, i);
        if(!validateJsonObject(colliderObj, json_type_string, "collider")) return 0;
        const char* colliderStr = json_object_get_string(colliderObj);
        size_t colliderLen = sizeof(colliderStr)/sizeof(colliderStr[0]);

        if(colliderLen!=1){
            fprintf(stderr, "wcoder error. In config file: ");
            fprintf(stderr, "colliderSymbol should be 1 character string.\n");
            return 0;
        }
        if(colliderStr[0] == junctionSymbolStr[0]){
            fprintf(stderr, "wcoder error. In config file: ");
            fprintf(stderr, "collider symbol and junction symbol cannot be same.\n");
            return 0;
        }
        if(colliderStr[0] == buildingSymbolStr[0]){
            fprintf(stderr, "wcoder error. In config file: ");
            fprintf(stderr, "collider symbol and building symbol cannot be same.\n");
            return 0;
        }
        *(map->colliders+i) = colliderStr[0];
    }

    
    //buidings
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
    //TODO: verify buildingsArr->length with text data
    map->buildings = malloc(sizeof(bidMap) * buildingsArr->length);

    //same value as i, just in uDynamInt to set bid & bidCount
    uDynamInt* iuDynam = createUDynamInt(sizeof(uint8_t));

    for(size_t i = 0; i<buildingsArr->length; i++){
        json_object* buildingObj = (json_object*)array_list_get_idx(buildingsArr, i);
        //TODO: add building number to title for easier debugging
        if(!validateJsonObject(buildingObj, json_type_object, "building")) return 0;

        //this foreach is there just to get the only 1 element
        json_object_object_foreach(buildingObj, buildingType, buildingDetailsObj){
            if(!validateJsonObject(buildingDetailsObj, json_type_object, "building details")) return 0;

            //copy iuDynam to bid
            (map->buildings + i)->bid = createUDynamInt(iuDynam->size);
            memcpy((map->buildings + i)->bid->base, iuDynam->base, sizeof(uint8_t) * iuDynam->size);

            if(!strcmp(buildingType, "FUNC")){
                json_object* opcodeListObj = json_object_object_get(buildingDetailsObj, "opcodeList");
                if(!validateJsonObject(opcodeListObj, json_type_array, "building opcodeList"))return 0;
                array_list* opcodeList = json_object_get_array(opcodeListObj);
                if(opcodeList->length == 0){
                    fprintf(stderr, "wcoder error. In config file: opcodeList cannot be empty.\n");
                    return 0;
                }
                
                uDynamInt* opcodesCount = sizeTToUDynamInt(opcodeList->length);
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
                        fprintf(stderr, "wcoder error. In config file: In building ");
                        printNum(iuDynam);
                        fprintf(stderr, "\n undefined opcode used: ");
                        fprintf(stderr, "%s.\n", opcodestr);
                        return 0;
                    }
                    if(opcode == 1 || opcode == 2){
                        fprintf(stderr, "wcoder error. In config file: In building ");
                        printNum(iuDynam);
                        fprintf(stderr, "\n%s", opcodestr);
                        fprintf(stderr, " cannot be used in opcodeList.\n");
                        return 0;
                    }
                    (map->buildings + i)->opcodes[j] = opcode;
                }
            }
            else if(!strcmp(buildingType, "REG")){
                json_object* baseAddressObj = json_object_object_get(buildingDetailsObj, "baseAddress");
                if(!validateJsonObject(baseAddressObj, json_type_string, "MEM building baseAddress")) return 0;
                const char* baseAddress = json_object_get_string(baseAddressObj);
                size_t baseAddressLength = sizeof(baseAddress)/sizeof(baseAddress[0]);

                if(baseAddressLength==0){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    printNum(iuDynam);
                    fprintf(stderr, "\nbaseAddress cannot be empty.\n");
                    return 0;
                }

                (map->buildings+i)->baseAddressBytes = baseAddressLength;

                (map->buildings+i)->baseAddress = strdup(baseAddress);
            }
            else if(!strcmp(buildingType, "MEM")){
                json_object* baseAddressObj = json_object_object_get(buildingDetailsObj, "baseAddress");
                if(!validateJsonObject(baseAddressObj, json_type_string, "MEM building baseAddress")) return 0;
                const char* baseAddress = json_object_get_string(baseAddressObj);
                size_t baseAddressLength = sizeof(baseAddress)/sizeof(baseAddress[0]);

                if(baseAddressLength==0){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    printNum(iuDynam);
                    fprintf(stderr, "\nbaseAddress cannot be empty.\n");
                    return 0;
                }

                (map->buildings+i)->baseAddressBytes = baseAddressLength;

                (map->buildings+i)->baseAddress = strdup(baseAddress);

                json_object* sizeObj = json_object_object_get(buildingDetailsObj, "size");
                if(!validateJsonObject(sizeObj, json_type_int, "size")) return 0;
                int64_t size = json_object_get_int64(sizeObj);
                
                
                if(size > subAddressMax){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    printNum(iuDynam);
                    fprintf(stderr, "\nMEM size too big. Max is: ");
                    fprintf(stderr, "%ld.\n", subAddressMax);
                    return 0;
                }
                if(size<0){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    printNum(iuDynam);
                    fprintf(stderr, "\nMEM size cannot be negative.\n");
                    return 0;
                }

                uDynamInt* sizeuDynam = sizeTToUDynamInt(size);
                (map->buildings+i)->memSize = sizeuDynam;
            }
            break;
        }
        iuDynam = incrementValUDynamInt(iuDynam);
    }
    map->bidCount = iuDynam;
    json_object_put(configObj);
    return 1;
}

mapData* initializeMapData(FILE* mapTextFile, json_object* configObj){
    mapData* map = (mapData*)malloc((sizeof(mapData)));
    map->buildingSymbol = '$'; // TODO: actually detect this in the below func.
    uint8_t configStatus = initializeConfigData(configObj, map);
    uint8_t mapStatus = initializeTextData(mapTextFile, map);
    if(mapStatus == 0 || configStatus == 0) return NULL;
    return map;
}
