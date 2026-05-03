CC = gcc

WARALIBS_CC_FLAGS = -Isrc/libs/include
WARALIBS_SOURCE = $(wildcard src/libs/*.c)

WMAP_CC_FLAGS = -Isrc/wmap/include $(WARALIBS_CC_FLAGS)
WMAP_ENCODER_SOURCE = $(wildcard src/wmap/*.c) $(WARALIBS_SOURCE)

JSONC_CFLAGS = $(shell pkg-config --cflags json-c)
JSONC_LIBS = $(shell pkg-config --libs json-c)


BIN_DIR = bin

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

wcoder: $(WMAP_ENCODER_SOURCE) | $(BIN_DIR)
	$(CC) $(WMAP_CC_FLAGS) $(JSONC_CFLAGS) $(WMAP_ENCODER_SOURCE) $(JSONC_LIBS) -o ./bin/$@

clean:
	rm -rf ./bin
	rm *.wmap
