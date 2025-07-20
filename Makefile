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
	./$(TARGET)

# Build and run tests
test: all
	./$(TEST_TARGET)

# Remove all build files and executables
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(TEST_TARGET)

# Clean and rebuild
rebuild: clean all
