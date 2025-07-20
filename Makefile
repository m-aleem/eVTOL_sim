BUILD_DIR := build
TARGET := eVTOL_sim
TEST_TARGET := eVTOL_tests

.PHONY: all clean rebuild run test

# Default build
all:
	cmake -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

# Run the main program
run: all
	./$(BUILD_DIR)/$(TARGET)

# Build and run tests
test: all
	cd $(BUILD_DIR) && ctest --output-on-failure

# Remove all build files
clean:
	rm -rf $(BUILD_DIR)

# Clean and rebuild
rebuild: clean all
