CC = gcc
WMAP_CC_FLAGS = -Isrc/wmap/include
WMAP_ENCODER_SOURCE = src/wmap/encoder.c src/wmap/messages.c
BIN_DIR = bin

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

wcoder: $(WMAP_ENCODER_SOURCE) | $(BIN_DIR)
	$(CC) $(WMAP_CC_FLAGS) $(WMAP_ENCODER_SOURCE) -o ./bin/$@

clean:
	rm -rf ./bin
