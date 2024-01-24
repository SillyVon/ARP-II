CC = gcc
CFLAGS = -lm -lncurses
SOURCE_DIR = src
BIN_DIR = bin

all: server_new main motion obstacles target_numbers watchdog

server_new: $(SOURCE_DIR)/server_.c
	$(CC) $< -o $(BIN_DIR)/server_new $(CFLAGS)

main: $(SOURCE_DIR)/main.c
	$(CC) $< -o $(BIN_DIR)/main $(CFLAGS)

motion: $(SOURCE_DIR)/motion_dynamics.c
	$(CC) $< -o $(BIN_DIR)/motion $(CFLAGS)

obstacles: $(SOURCE_DIR)/obstacles_spawner.c
	$(CC) $< -o $(BIN_DIR)/obstacles $(CFLAGS)

target_numbers: $(SOURCE_DIR)/target_numbers.c
	$(CC) $< -o $(BIN_DIR)/target_numbers $(CFLAGS)

watchdog: $(SOURCE_DIR)/watchdog.c
	$(CC) $< -o $(BIN_DIR)/watchdog $(CFLAGS)

clean:
	rm -f $(BIN_DIR)/*

run: server_new
	./$(BIN_DIR)/server_new

