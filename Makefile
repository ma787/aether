SRC_DIR := ./src
TEST_DIR := ./tests
INC_DIR := ./include
BUILD_DIR := ./build

test:
	mkdir -p $(BUILD_DIR)
	gcc -c $(SRC_DIR)/board.c -I $(INC_DIR) -o $(BUILD_DIR)/board.o
	gcc -c $(SRC_DIR)/constants.c -I $(INC_DIR) -o $(BUILD_DIR)/constants.o
	gcc -c $(SRC_DIR)/fen_parser.c -I $(INC_DIR) -o $(BUILD_DIR)/fen_parser.o
	gcc -c $(TEST_DIR)/test_fen_parser.c -I $(INC_DIR) -o $(BUILD_DIR)/test_fen_parser.o
	gcc -o $(BUILD_DIR)/test_fen_parser $(BUILD_DIR)/board.o $(BUILD_DIR)/constants.o $(BUILD_DIR)/fen_parser.o $(BUILD_DIR)/test_fen_parser.o
	$(BUILD_DIR)/test_fen_parser

clean:
	rm -r $(BUILD_DIR)
