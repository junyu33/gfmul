# Project-specific settings
BENCH = bench
HEADER ?= GFMUL
BITS ?= 128
SOURCES = bench.cpp
GEN_SOURCE = gen.cpp
BIN_DIR = .

# Include common logic
include ../config/common.mk

# Build all
all: $(BIN_DIR) $(TARGET)

# Build the target executable
$(TARGET): $(SOURCES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# Generate input
gen: 
	g++ -o $(BIN_DIR)/gen $(GEN_SOURCE)	
	$(BIN_DIR)/gen $(BITS)

# Run the executable
run: $(TARGET)
	$(RUN_CMD)

clean: 
	rm -rf $(BIN_DIR)/$(BENCH)_*

clean-all: clean
	rm -rf $(BIN_DIR)/gen
