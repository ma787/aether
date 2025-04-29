SRC_DIR := ./src
TEST_DIR := ./tests
BUILD_DIR := ./build
EXEC_DIR := ./bin
SRCS := $(wildcard $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(SRC_DIR)/*.h)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
EXE := $(EXEC_DIR)/aether

DEBUG_FLAGS := -g -Wall

all: $(EXE)

$(EXE): $(OBJS) | $(EXEC_DIR)
	gcc $(DEBUG_FLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR) $(HEADERS)
	gcc $(DEBUG_FLAGS) -c $^ -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EXEC_DIR):
	mkdir -p $(EXEC_DIR)

test: $(TEST_DIR)/tests.c | $(OBJS) $(EXEC_DIR)
	gcc $(DEBUG_FLAGS) -c $^ -I $(SRC_DIR) -o $(BUILD_DIR)/tests.o
	gcc $(BUILD_DIR)/tests.o $(filter-out $(BUILD_DIR)/uci.o, $(OBJS)) -o $(EXEC_DIR)/tests

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(EXEC_DIR)
