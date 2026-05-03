CC = gcc

WARALIBS_CC_FLAGS = -Ilib/include
WARALIBS_SOURCE = $(wildcard lib/*.c)

WMAP_CC_FLAGS = -Isrc/wmap/include $(WARALIBS_CC_FLAGS)
WMAP_ENCODER_SOURCE = $(wildcard src/wmap/*.c) $(WARALIBS_SOURCE)

JSONC_CFLAGS = $(shell pkg-config --cflags json-c)
JSONC_LIBS = $(shell pkg-config --libs json-c)

BIN_DIR = bin

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

wcoder: $(WMAP_ENCODER_SOURCE) | $(BIN_DIR)
	$(CC) $(WMAP_CC_FLAGS) $(JSONC_CFLAGS) $(WMAP_ENCODER_SOURCE) $(JSONC_LIBS) -o ./bin/$@

run: wcoder
	./bin/wcoder ./src/templates/nitwMap.txt ./src/templates/wmapConfig.json

clean:
	rm -rf ./bin
	rm *.wmap
