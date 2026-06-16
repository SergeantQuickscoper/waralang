#include <encoderSub.h>
#include <math.h>

// There should be max 256 elements in this array - unvalidated
// There must be "MEM" and "REG" in this array - unvalidated
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

/*
    This function would honestly be pretty useful for debugging when trying to
    create your own wmap. TODO: maybe look into adding this as a flag to output
    the wmap into a file.
*/
void printCurrMapMatrix(mapData* mapData){
    if(mapData == NULL){
        fprintf(stderr, "wmap encoder error while printing map!"
        );
    }
    size_t heightS = uDynamIntToSizeT(mapData->height);
    size_t widthS = uDynamIntToSizeT(mapData->width);
    size_t counter = 0;
    FILE* out = fopen("./currMapMatrix.txt", "w"); // hard
    for(size_t i = 0; i < heightS; i++){
        for(size_t j = 0; j < widthS; j++){
            if(counter == widthS){
                counter = 0;
                char new = '\n';
                fwrite(&new, sizeof(char), 1, out);
            }
            if((mapData->mapMatrix + widthS * i + j)->symbol == 0){
                uDynamInt* bid = (mapData->mapMatrix + widthS * i + j)->bid;
                size_t temp = uDynamIntToSizeT(bid);
                char toPrint = 62 + temp;
                fwrite(&toPrint, sizeof(char), 1, out);
            }
            else{
                fwrite(&((mapData->mapMatrix + widthS * i + j)->symbol),
                sizeof(char), 1, out);
            }
            counter++;
        }
    }
    fclose(out);
}

uint8_t fillBuildings(mapData* mapData){
    if(mapData == NULL || mapData->height == NULL || mapData->width == NULL){
        fprintf(stderr, "wmap encoder error! uninitialized values during DFS!"
        );
        return 0;
    }
    size_t heightS = uDynamIntToSizeT(mapData->height);
    size_t widthS = uDynamIntToSizeT(mapData->width);
    uDynamInt* bidCount = createUDynamInt(sizeof(uint8_t));
    for(size_t i = 0; i < heightS; i++){
        for(size_t j = 0; j < widthS; j++){
            if((mapData->mapMatrix + i * widthS + j)->symbol ==
            mapData->buildingSymbol){
                bidCount = incrementValUDynamInt(bidCount);
                uDynamInt* currInd = copyUDynamInt(bidCount);
                dfs(mapData, currInd, heightS, widthS, j, i);
            }
        }
    }
    if(isEqual(mapData->bidCount, bidCount)==0){
        fprintf(stderr, "wmap encoder error! Number of buildings in ascii");
        fprintf(stderr, " map: ");
        printNum(bidCount);
        fprintf(stderr, ", do not match number of buildings in config: ");
        printNum(mapData->bidCount);
        fprintf(stderr, "\n");
        return 0;
    }

    /*
                Reserved bids (as per file format specification):
                Traversables: MAX_BID
                Colliders: MAX_BID - 1
                Junctions: MAX_BID - 2
    */
    for(size_t i = 0; i < heightS; i++){
        for(size_t j = 0; j < widthS; j++){
            char thisSym = (mapData->mapMatrix + i * widthS + j)->symbol;
            size_t bidMax = (1 << ((mapData->bidCount->size) * 8)) - 1;
            if(thisSym == mapData->junctionSymbol){
                (mapData->mapMatrix + i * widthS + j)->bid = sizeTToUDynamInt(bidMax - 2);
            }
            else{
                uint8_t isColl = 0;
                for(uint8_t k = 0; k < mapData->collidersBytes; k++){
                    if(thisSym == mapData->colliders[k]){
                        (mapData->mapMatrix + i * widthS + j)->bid = sizeTToUDynamInt(bidMax - 1);
                        isColl = 1;
                        break;
                    }
                }
                if(isColl == 0){
                    (mapData->mapMatrix + i * widthS + j)->bid = sizeTToUDynamInt(bidMax);
                }
            }
        }
    }
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
    size_t heightAlloc = uDynamIntToSizeT(height);
    size_t widthAlloc = uDynamIntToSizeT(width);
    if(heightAlloc == 0 || widthAlloc == 0){
        fprintf(stderr, "wmap error. Error with uDynamInt and map dimensions");
        return 0;
    }
    // TODO: manage this overflow risk from multiplying size_t's
    map->height = height;
    map->width = width;
    map->mapMatrix = (mapCell*)malloc(sizeof(mapCell) * heightAlloc *
    widthAlloc);
    map->spawnX = setSizeUDynamInt(map->spawnX, map->width->size);
    map->spawnY = setSizeUDynamInt(map->spawnY, map->height->size);
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
    uint8_t fillStatus = fillBuildings(map);
    if(fillStatus == 0){
        return 0;
    }
    return 1;
}


uint8_t validateJsonObject(json_object* obj, enum json_type desiredType, char* name){
    if(obj == NULL){
        fprintf(stderr, "wcoder error. In config file: ");
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

/*
    The following function currently does not perform necessary checks
    when downcasting to size_t from int_64. Therefore on systems lower
    than 64 bit a possibility of overflow does exist. Fixing this is a
    TODO for now.
*/
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

    /*
        spawnX and spawnY are compared to and resized to width and height,
        in the intiialize text function.
    */
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
    if(baseAddressSizeInt64 < 0){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "baseAddressSize cannot be negative.\n");
        return 0;
    }
    // because we are storing number of MEM buildings in size_t
    if(baseAddressSizeInt64 > 8 * sizeof(size_t) - 1){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "baseAddressSize cannot be greater than size_t size.\n");
        return 0;
    }
    size_t baseAddressSizeSizeT = (size_t)baseAddressSizeInt64;
    uDynamInt* baseAddressSizeUDynam = sizeTToUDynamInt(baseAddressSizeSizeT);
    map->baseAddressSize = baseAddressSizeUDynam;
    size_t baseAddressMax = ((size_t)1<<baseAddressSizeSizeT)-1;

    json_object* subAddressSizeObj = json_object_object_get(wordSizeObj, "subAddressSize");
    if(!validateJsonObject(subAddressSizeObj, json_type_int, "subAddressSize")) return 0;
    int64_t subAddressSizeInt64 = json_object_get_int64(subAddressSizeObj);
    if(subAddressSizeInt64<0){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "subAddressSize cannot be negative.\n");
        return 0;
    }
    //because we are storing storage size of `MEM` buildings in an int64
    if(subAddressSizeInt64 > 8 * sizeof(size_t) - 1){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "subAddressSize cannot be greater than size_t size\n");
        return 0;
    }
    size_t subAddressSizeSizeT = (size_t)subAddressSizeInt64;
    uDynamInt* subAddressSizeUDynam = sizeTToUDynamInt(subAddressSizeSizeT);
    map->subAddressSize = subAddressSizeUDynam;
    size_t subAddressMax = ((size_t)1<<subAddressSizeSizeT)-1;

    //symbols
    json_object* buildingSymbolObj = json_object_object_get(configObj, "buildingSymbol");
    if(!validateJsonObject(buildingSymbolObj, json_type_string, "buildingSymbol")) return 0;
    const char* buildingSymbolStr = json_object_get_string(buildingSymbolObj);
    size_t buildingSymbolLen = json_object_get_string_len(buildingSymbolObj);
    if(buildingSymbolLen!=1){
        fprintf(stderr, "wcoder error. In config file: ");
        fprintf(stderr, "buildingSymbol should be 1 character string.\n");
        return 0;
    }
    map->buildingSymbol = buildingSymbolStr[0];

    json_object* junctionSymbolObj = json_object_object_get(configObj, "junctionSymbol");
    if(!validateJsonObject(junctionSymbolObj, json_type_string, "junctionSymbol")) return 0;
    const char* junctionSymbolStr = json_object_get_string(junctionSymbolObj);
    size_t junctionSymbolLen = json_object_get_string_len(junctionSymbolObj);
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
    if(!validateJsonObject(collidersObj, json_type_array, "colliders")) return 0;
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
        size_t colliderLen = json_object_get_string_len(colliderObj);

        if(colliderLen!=1){
            fprintf(stderr, "wcoder error. In config file: ");
            fprintf(stderr, "colliderSymbols should be 1 character string each.\n");
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

    const uint8_t ALLOWEDOPCODESLENGTH = sizeof(ALLOWEDOPCODES)/sizeof(ALLOWEDOPCODES[0]);

    map->buildings = malloc(sizeof(bidMap) * buildingsArr->length);
    size_t memCount = 0;
    size_t i;

    Trie* memRegNamesTrie = createTrie();

    for(i = 0; i<buildingsArr->length; i++){
        json_object* buildingObj = (json_object*)array_list_get_idx(buildingsArr, i);
        //TODO: add building number to title for easier debugging
        if(!validateJsonObject(buildingObj, json_type_object, "building")) return 0;

        //this foreach is there just to get the only 1 element
        json_object_object_foreach(buildingObj, buildingType, buildingDetailsObj){
            map->buildings[i].buildingType = FUNCTYPE;
            if(!validateJsonObject(buildingDetailsObj, json_type_object, "building details")) return 0;

            if(!strcmp(buildingType, "FUNC")){
                json_object* opcodeListObj = json_object_object_get(buildingDetailsObj, "opcodeList");
                if(!validateJsonObject(opcodeListObj, json_type_array, "building opcodeList"))return 0;
                array_list* opcodeList = json_object_get_array(opcodeListObj);
                if(opcodeList->length == 0){
                    fprintf(stderr, "wcoder error. In config file: opcodeList cannot be empty.\n");
                    return 0;
                }

                uDynamInt* opcodesCount = sizeTToUDynamInt(opcodeList->length);
                map->buildings[i].buildingData.func.opcodeCount = opcodesCount;

                map->buildings[i].buildingData.func.opcodes = malloc( opcodeList->length * sizeof(uint8_t) );

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
                        fprintf(stderr, "%zu\n undefined opcode used: ", i);
                        fprintf(stderr, "%s.\n", opcodestr);
                        return 0;
                    }
                    if(opcode == 1 || opcode == 2){
                        fprintf(stderr, "wcoder error. In config file: In building ");
                        fprintf(stderr, "%zu", i);
                        fprintf(stderr, "\n%s", opcodestr);
                        fprintf(stderr, " cannot be used in opcodeList.\n");
                        return 0;
                    }
                    map->buildings[i].buildingData.func.opcodes[j] = opcode;
                }
            }
            else if(!strcmp(buildingType, "REG")){
                map->buildings[i].buildingType = REGTYPE;
                json_object* regNameObj = json_object_object_get(buildingDetailsObj, "regName");
                if(!validateJsonObject(regNameObj, json_type_string, "REG building regName")) return 0;
                const char* regName = json_object_get_string(regNameObj);
                size_t regNameLength = json_object_get_string_len(regNameObj);

                if(regNameLength==0){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\nregName cannot be empty.\n", i);
                    return 0;
                }

                void* insertResult = insertElementTrie(memRegNamesTrie, regName, NULL);
                if(insertResult == (void*)-1){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\n", i);
                    return 0;
                }
                if(insertResult != memRegNamesTrie->notEndPtr){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\n", i);
                    fprintf(stderr, "all MEM regNamees and REG regNames should be exclusive.\n");
                    return 0;
                }

                map->buildings[i].buildingData.reg.regNameBytes = regNameLength;

                map->buildings[i].buildingData.reg.regName = strdup(regName);
            }
            else if(!strcmp(buildingType, "MEM")){
                map->buildings[i].buildingType = MEMTYPE;
                if(memCount > baseAddressMax){
                    fprintf(stderr, "wcoder error. In config file: ");
                    fprintf(stderr, "number of MEM buildings exceed baseAddress wordSize capacity.");
                    fprintf(stderr, "current capacity: ");
                    fprintf(stderr, "%zu\n", baseAddressMax);
                    fprintf(stderr, "increase baseAddressSize to increase capacity.");
                    return 0;
                }
                memCount++;
                json_object* baseAddressObj = json_object_object_get(buildingDetailsObj, "baseAddress");
                if(!validateJsonObject(baseAddressObj, json_type_string, "MEM building baseAddress")) return 0;
                const char* baseAddress = json_object_get_string(baseAddressObj);
                size_t baseAddressLength = json_object_get_string_len(baseAddressObj);

                if(baseAddressLength==0){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\nbaseAddress cannot be empty.\n", i);
                    return 0;
                }
                void* insertResult = insertElementTrie(memRegNamesTrie, baseAddress, NULL);
                if(insertResult == (void*)-1){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\n", i);
                    return 0;
                }
                if(insertResult != memRegNamesTrie->notEndPtr){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\n", i);
                    fprintf(stderr, "all MEM baseAddresses and REG regNames should be exclusive.\n");
                    return 0;
                }

                map->buildings[i].buildingData.mem.baseAddressBytes = baseAddressLength;

                map->buildings[i].buildingData.mem.baseAddress = strdup(baseAddress);

                json_object* sizeObj = json_object_object_get(buildingDetailsObj, "size");
                if(!validateJsonObject(sizeObj, json_type_int, "size")) return 0;
                int64_t size = json_object_get_int64(sizeObj);

                if(size > subAddressMax){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\nMEM size too big. Max is: ", i);
                    fprintf(stderr, "%zu.\n", subAddressMax);
                    fprintf(stderr, "increase subAddressSize to increase capacity.\n");
                    return 0;
                }
                if(size<0){
                    fprintf(stderr, "wcoder error. In config file: In building ");
                    fprintf(stderr, "%zu\nMEM size cannot be negative.\n", i);
                    return 0;
                }

                uDynamInt* sizeuDynam = sizeTToUDynamInt(size);
                map->buildings[i].buildingData.mem.memSize = sizeuDynam;
            }
            break;
        }
    }
    map->bidCount = sizeTToUDynamInt(i);
    json_object_put(configObj);
    return 1;
}

mapData* initializeMapData(FILE* mapTextFile, json_object* configObj){
    mapData* map = (mapData*)malloc((sizeof(mapData)));
    uint8_t configStatus = initializeConfigData(configObj, map);
    uint8_t mapStatus = initializeTextData(mapTextFile, map);
    if(mapStatus == 0 || configStatus == 0) return NULL;
    return map;
}

uint8_t encodeData(char* outPath, mapData* map){
    if(strlen(outPath) == 0){
        return 0;
    }
    FILE* outFile;
    char* finOut = outPath;
    uint8_t allocFlag = 0;
    if(outPath[strlen(outPath) - 1] == '/'){
        char* defaultName = "out.wmap";
        finOut = (char*)malloc(sizeof(char) * (strlen(defaultName) + strlen(outPath) + 1));
        strcpy(finOut, outPath);
        strcat(finOut, defaultName);
        allocFlag = 1;
    }
    outFile = fopen(finOut, "wb");

    fwrite(&map->height->size, sizeof(map->height->size), 1, outFile);
    fwrite(&map->width->size, sizeof(map->width->size), 1, outFile);

    size_t bidCount = uDynamIntToSizeT(map->bidCount);
    int bits = 0;
    size_t temp = bidCount;
    while(temp != 0){
        bits++;
        temp = temp >> 1;
    }
    if(bits == 0) bits = 1;
    uint8_t bidBytes = (bits + 7)/8;
    fwrite(map->height->base, sizeof(uint8_t), map->height->size, outFile);
    fwrite(map->width->base, sizeof(uint8_t), map->width->size, outFile);
    fwrite(&bidBytes, sizeof(uint8_t), 1, outFile);
    fwrite(map->spawnX->base, sizeof(uint8_t), map->spawnX->size, outFile);
    fwrite(map->spawnY->base, sizeof(uint8_t), map->spawnY->size, outFile);
    fwrite(&map->spawnDirection, sizeof(char), 1, outFile);
    uint8_t max = (map->baseAddressSize->size > map->subAddressSize->size) ?
    map->baseAddressSize->size : map->subAddressSize->size;
    fwrite(&max, sizeof(uint8_t), 1, outFile);
    fwrite(map->baseAddressSize->base, sizeof(uint8_t), max, outFile);
    fwrite(map->subAddressSize->base, sizeof(uint8_t), max, outFile);

    size_t heightS = uDynamIntToSizeT(map->height);
    size_t widthS = uDynamIntToSizeT(map->width);
    for(size_t i = 0; i < heightS; i++){
        for(size_t j = 0; j < widthS; j++){
                fwrite(&((map->mapMatrix + widthS * i + j)->symbol),
                sizeof(char), 1, outFile);
                (map->mapMatrix + widthS * i + j)->bid = setSizeUDynamInt(
                    (map->mapMatrix + widthS * i + j)->bid, bidBytes);
                fwrite((map->mapMatrix + widthS * i + j)->bid->base,
                sizeof(uint8_t), bidBytes, outFile);
        }
    }

    const size_t ALLOWEDOPCODESLENGTH = sizeof(ALLOWEDOPCODES)/sizeof(ALLOWEDOPCODES[0]);
    size_t memOpcode = 0, regOpcode = 0;
    for(size_t i = 0; i<ALLOWEDOPCODESLENGTH; i++){
        if(strcmp(ALLOWEDOPCODES[i], "REG") == 0){
            regOpcode = i;
        }
        else if(strcmp(ALLOWEDOPCODES[i], "MEM") == 0){
            memOpcode = i;
        }
    }

    for(size_t i = 0; i<bidCount; i++){
        if(map->buildings[i].buildingType == FUNCTYPE){
            fputc(map->buildings[i].buildingData.func.opcodeCount->size,
                outFile);
            fwrite(map->buildings[i].buildingData.func.opcodeCount->base,
                sizeof(uint8_t),
                map->buildings[i].buildingData.func.opcodeCount->size,
                outFile);
            fwrite(map->buildings[i].buildingData.func.opcodes,
                sizeof(uint8_t),
            uDynamIntToSizeT(map->buildings[i].buildingData.func.opcodeCount),
                outFile);
        }
        else if(map->buildings[i].buildingType == REGTYPE){
            fputc(1, outFile);
            fputc(1, outFile);
            fputc(regOpcode, outFile);
            fputc(map->buildings[i].buildingData.reg.regNameBytes, outFile);
            fwrite(map->buildings[i].buildingData.reg.regName, sizeof(char),
                map->buildings[i].buildingData.reg.regNameBytes, outFile);
        }
        else if(map->buildings[i].buildingType == MEMTYPE){
            fputc(1, outFile);
            fputc(1, outFile);
            fputc(memOpcode, outFile);
            fputc(map->buildings[i].buildingData.mem.baseAddressBytes,
                outFile);
            fwrite(map->buildings[i].buildingData.mem.baseAddress,
                sizeof(char),
                map->buildings[i].buildingData.mem.baseAddressBytes, outFile);
            fputc(map->buildings[i].buildingData.mem.memSize->size, outFile);
            fwrite(map->buildings[i].buildingData.mem.memSize->base,
                sizeof(uint8_t),
                map->buildings[i].buildingData.mem.memSize->size, outFile);
        }
    }

    fclose(outFile);
    if(allocFlag == 1) free(finOut);
    return 1;
}