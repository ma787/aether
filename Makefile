SRC_DIR := ./src
TEST_DIR := ./tests
INC_DIR := ./include
BUILD_DIR := ./build
DEBUG_FLAGS := -Wall

test:
	mkdir -p $(BUILD_DIR)
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/position.c -I $(INC_DIR) -o $(BUILD_DIR)/position.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/constants.c -I $(INC_DIR) -o $(BUILD_DIR)/constants.o
	gcc -c $(DEBUG_FLAGS) $(TEST_DIR)/test_position.c -I $(INC_DIR) -o $(BUILD_DIR)/test_position.o
	gcc -o $(BUILD_DIR)/test_position $(BUILD_DIR)/constants.o $(BUILD_DIR)/position.o $(BUILD_DIR)/test_position.o
	$(BUILD_DIR)/test_position

clean:
	rm -r $(BUILD_DIR)
