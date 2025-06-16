# Compiler and standard flags
CC = cc
CFLAGS_COMMON = -std=c23 -g -fPIC -pedantic -Wall -Wextra -Wconversion -Wno-unused-parameter
# -fsanitize=address,undefined # -fno-omit-frame-pointer
CFLAGS_OPTIMIZED = -O3 -flto
CFLAGS_DEBUG = -g -Wno-unused-function
CFLAGS_LINK = -lm

# Targets
TARGET = a.out

# Directories
SRC_DIR = src
TEST_DIR = test

# Source and object files
SRCS = $(shell find $(SRC_DIR) -name '*.c' -and -not -name 'main.c')
OBJS = $(patsubst %.c, %.o, $(SRCS))

TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_EXEC = test.out

# Default target
all: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_OPTIMIZED) $(CFLAGS_LINK)
all: $(TARGET)

# Debug target
debug: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG) $(CFLAGS_LINK)
debug: $(TARGET)

# Test target
test: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_LINK)
test: $(TEST_EXEC)

# Compile source files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $^ -o $@

# Test executable
$(TEST_EXEC): $(TEST_OBJ) $(OBJS)
	$(CC) $(CFLAGS) $^ -lcriterion -o $@ && ./$(TEST_EXEC) --verbose=1

# Compile test files
$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJ) $(TEST_EXEC) $(SRC_DIR)/main.o

# Phony targets
.PHONY: all debug clean
