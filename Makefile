CC = gcc
WMAP_CC_FLAGS = -Isrc/wmap/include
JSONC_CFLAGS = $(shell pkg-config --cflags json-c)
JSONC_LIBS = $(shell pkg-config --libs json-c)

WMAP_ENCODER_SOURCE = $(wildcard src/wmap/*.c)

BIN_DIR = bin

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

wcoder: $(WMAP_ENCODER_SOURCE) | $(BIN_DIR)
	$(CC) $(WMAP_CC_FLAGS) $(JSONC_CFLAGS) $(JSONC_LIBS) $(WMAP_ENCODER_SOURCE) -o ./bin/$@

clean:
	rm -rf ./bin
	rm *.wmap
