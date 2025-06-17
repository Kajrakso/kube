# Compiler and standard flags
CC = cc
CFLAGS_COMMON = -std=c2x -g -fPIC -pedantic -Wall -Wextra -Wconversion -Wno-unused-parameter
# -fsanitize=address,undefined # -fno-omit-frame-pointer
CFLAGS_OPTIMIZED = -O3 -flto
CFLAGS_DEBUG = -g -Wno-unused-function
CFLAGS_LINK = -lm

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

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
all: kube

# Debug target
debug: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG) $(CFLAGS_LINK)
debug: kube

# Test target
test: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_LINK)
test: $(TEST_EXEC)

# Compile source files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kube: $(OBJS) $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $^ -o $@

# Test executable
$(TEST_EXEC): $(TEST_OBJ) $(OBJS)
	$(CC) $(CFLAGS) $^ -lcriterion -o $@ && ./$(TEST_EXEC) --verbose=1

# Compile test files
$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f kube ${DESTDIR}${PREFIX}/bin/kube
	chmod 755 ${DESTDIR}${PREFIX}/bin/kube
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	# sed "s/VERSION/${VERSION}/g" < doc/kube.1 > ${DESTDIR}${MANPREFIX}/man1/kube.1
	cp doc/kube.1 ${DESTDIR}${MANPREFIX}/man1/kube.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/kube.1

uninstall:
	rm -rf ${DESTDIR}${PREFIX}/bin/kube ${DESTDIR}${MANPREFIX}/man1/kube.1

# Clean target
clean:
	rm -f $(OBJS) kube $(TEST_OBJ) $(TEST_EXEC) $(SRC_DIR)/main.o

# Phony targets
.PHONY: all debug clean install uninstall
