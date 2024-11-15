# Project-specific settings
BENCH = gfmul
HEADER ?= GFMUL
SOURCES = $(wildcard src/*.cpp)

# Include common logic
include config/common.mk

# Build all
all: $(BIN_DIR) $(TARGET)

# Build the target executable
$(TARGET): $(SOURCES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Run the executable
run: $(TARGET)
	$(RUN_CMD)

clean:
	rm -rf $(BIN_DIR)
