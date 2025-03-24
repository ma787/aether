SRC_DIR := ./src
TEST_DIR := ./tests
INC_DIR := ./include
BUILD_DIR := ./build
DEBUG_FLAGS := -g -Wall

aether:
	mkdir -p $(BUILD_DIR)
	gcc $(DEBUG_FLAGS) $(SRC_DIR)/*.c -I $(INC_DIR) -o $(BUILD_DIR)/aether

test:
	mkdir -p $(BUILD_DIR)
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/constants.c -I $(INC_DIR) -o $(BUILD_DIR)/constants.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/utils.c -I $(INC_DIR) -o $(BUILD_DIR)/utils.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/position.c -I $(INC_DIR) -o $(BUILD_DIR)/position.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/move.c -I $(INC_DIR) -o $(BUILD_DIR)/move.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/hashing.c -I $(INC_DIR) -o $(BUILD_DIR)/hashing.o
	gcc -c $(DEBUG_FLAGS) $(TEST_DIR)/tests.c -I $(INC_DIR) -o $(BUILD_DIR)/tests.o
	gcc -o $(BUILD_DIR)/tests $(BUILD_DIR)/*.o

clean:
	rm -r $(BUILD_DIR)
