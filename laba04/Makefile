CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -ldl

SRC_DIR = .
BUILD_DIR = build

SRC_FILES = dynamic_lib.c lib_first.c lib_second.c
OBJ_FILES = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_FILES))
LIB_FILES = $(patsubst %.c, $(BUILD_DIR)/lib%.so, $(filter-out dynamic_lib.c, $(SRC_FILES)))

TARGET = $(BUILD_DIR)/dynamic_lib

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES) $(LIB_FILES)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lib%.so: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

clean:
	rm -rf $(BUILD_DIR)
