SRC_DIR := ./src
TEST_DIR := ./tests
INC_DIR := ./include
BUILD_DIR := ./build
DEBUG_FLAGS := -Wall

test-position:
	mkdir -p $(BUILD_DIR)
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/position.c -I $(INC_DIR) -o $(BUILD_DIR)/position.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/constants.c -I $(INC_DIR) -o $(BUILD_DIR)/constants.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/utils.c -I $(INC_DIR) -o $(BUILD_DIR)/utils.o
	gcc -o $(BUILD_DIR)/test_position $(BUILD_DIR)/constants.o $(BUILD_DIR)/position.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/test_position.o
	$(BUILD_DIR)/test_position

test-move:
	mkdir -p $(BUILD_DIR)
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/position.c -I $(INC_DIR) -o $(BUILD_DIR)/position.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/constants.c -I $(INC_DIR) -o $(BUILD_DIR)/constants.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/move.c -I $(INC_DIR) -o $(BUILD_DIR)/move.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/utils.c -I $(INC_DIR) -o $(BUILD_DIR)/utils.o
	gcc -c $(DEBUG_FLAGS) $(TEST_DIR)/test_move.c -I $(INC_DIR) -o $(BUILD_DIR)/test_move.o
	gcc -o $(BUILD_DIR)/test_move $(BUILD_DIR)/constants.o $(BUILD_DIR)/position.o $(BUILD_DIR)/move.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/test_move.o	
	$(BUILD_DIR)/test_move

test-move-gen:
	mkdir -p $(BUILD_DIR)
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/position.c -I $(INC_DIR) -o $(BUILD_DIR)/position.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/constants.c -I $(INC_DIR) -o $(BUILD_DIR)/constants.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/move.c -I $(INC_DIR) -o $(BUILD_DIR)/move.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/move_gen.c -I $(INC_DIR) -o $(BUILD_DIR)/move_gen.o
	gcc -c $(DEBUG_FLAGS) $(SRC_DIR)/utils.c -I $(INC_DIR) -o $(BUILD_DIR)/utils.o
	gcc -c $(DEBUG_FLAGS) $(TEST_DIR)/test_move_gen.c -I $(INC_DIR) -o $(BUILD_DIR)/test_move_gen.o
	gcc -o $(BUILD_DIR)/test_move_gen $(BUILD_DIR)/constants.o $(BUILD_DIR)/position.o $(BUILD_DIR)/move.o $(BUILD_DIR)/move_gen.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/test_move_gen.o
	$(BUILD_DIR)/test_move_gen

test: $(BUILD_DIR)/test_position $(BUILD_DIR)/test_move $(BUILD_DIR)/test_move_gen
	$(BUILD_DIR)/test_position
	$(BUILD_DIR)/test_move
	$(BUILD_DIR)/test_move_gen

clean:
	rm -r $(BUILD_DIR)
