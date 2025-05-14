CC = gcc
LD_FLAGS = -lSDL2 -lchipmunk -lm -I/usr/include/SDL2
CFLAGS = -O2 

SRC_DIR = src
BUILD_DIR = build
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))
TARGET = $(BUILD_DIR)/particles

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -vp $@

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LD_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@./$(TARGET)

clean:
	@rm -rvf $(BUILD_DIR) $(TARGET)

.PHONY: all clean run
