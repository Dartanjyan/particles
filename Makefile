CC_LINUX = gcc
CC_WIN = x86_64-w64-mingw32-gcc

LDFLAGS = -lchipmunk -lm
LDFLAGS_LINUX = $(LDFLAGS) -lSDL2 -L/usr/local/lib
LDFLAGS_WIN = $(LDFLAGS) -L3rdparty/SDL/install/lib -lmingw32 -lSDL2main -lSDL2 -mwindows -L3rdparty/chipmunk/install/lib -lwinpthread

CFLAGS = -O2
CFLAGS_LINUX = $(CFLAGS) -I/usr/include/SDL2 -D_REENTRANT
CFLAGS_WIN = $(CFLAGS) -I3rdparty/SDL/install/include -I3rdparty/chipmunk/install/include -Dmain=SDL_main

SRC_DIR = src
BUILD_DIR = build
SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ_LINUX = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o.l, $(SRC))
OBJ_WIN = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o.w, $(SRC))
BIN_NAME = particles
TARGET_LINUX = $(BUILD_DIR)/$(BIN_NAME)
TARGET_WIN = $(BUILD_DIR)/$(BIN_NAME).exe

linux: $(BUILD_DIR) $(TARGET_LINUX)
#	tar -cJvf $(BUILD_DIR)/$(BIN_NAME).tar.xz $(TARGET_LINUX)

windows: $(BUILD_DIR) $(TARGET_WIN)
	@cp -v 3rdparty/SDL/install/bin/SDL2.dll $(BUILD_DIR)
	@bash -c "if [[ -f /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll ]]; then cp -v /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll $(BUILD_DIR); else cp -v /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll $(BUILD_DIR); fi"
	@zip $(BUILD_DIR)/$(BIN_NAME).zip $(TARGET_WIN) $(BUILD_DIR)/*.dll

all: linux windows
	
win: windows

$(BUILD_DIR):
	@mkdir -vp $@

$(TARGET_LINUX): $(OBJ_LINUX)
	$(CC_LINUX) $^ -o $@ $(LDFLAGS_LINUX)

$(TARGET_WIN): $(OBJ_WIN)
	$(CC_WIN) $^ -o $@ $(LDFLAGS_WIN)

$(BUILD_DIR)/%.o.l: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC_LINUX) $(CFLAGS_LINUX) -c $< -o $@

$(BUILD_DIR)/%.o.w: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC_WIN) $(CFLAGS_WIN) -c $< -o $@



run: $(TARGET_LINUX)
	@./$(TARGET_LINUX)

clean:
	@rm -rvf $(BUILD_DIR)

.PHONY: all clean run linux windows win
