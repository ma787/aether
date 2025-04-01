SRC_DIR := ./src
TEST_DIR := ./tests
BUILD_DIR := ./build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
EXE := $(BUILD_DIR)/aether

DEBUG_FLAGS := -g -Wall

all: $(EXE)

$(EXE): $(OBJS) 
	gcc $(DEBUG_FLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	gcc $(DEBUG_FLAGS) -c $^ -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(TEST_DIR)/tests.c | $(OBJS)
	gcc $(DEBUG_FLAGS) -c $^ -I $(SRC_DIR) -o $(BUILD_DIR)/tests.o
	gcc -o $(BUILD_DIR)/tests $(BUILD_DIR)/tests.o $(filter-out $(BUILD_DIR)/uci.o, $(OBJS))

clean:
	rm -r $(BUILD_DIR)
